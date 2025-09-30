//
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
/*
 * Questa versione della classe è stata modificata nella funzione di computePowerConsumption() rispetto
 * all'implementazione originale per tenere traccia separata dei consumi:
 *   - powerConsumption: consumo totale (somma di RX e TX e altri stati)
 *   - powerConsumptionRx: consumo dovuto solo alla ricezione
 *   - powerConsumptionTx: consumo dovuto solo alla trasmissione
 *
 * Il metodo ritorna un array di tre elementi W:
 *   0: consumo totale
 *   1: consumo ricezione (RX)
 *   2: consumo trasmissione (TX)
 *
 * Questa modifica permette analisi dettagliate del consumo energetico
 * per fase di comunicazione, senza alterare la compatibilità con altri
 * componenti INET che leggono solo il consumo totale.
 **/

#include "../MY_EnergyConsumer/MY_StateBasedEpEnergyConsumer.h"

namespace inet {
namespace physicallayer {

Define_Module(MY_StateBasedEpEnergyConsumer);

void MY_StateBasedEpEnergyConsumer::initialize(int stage) {
    // prima chiamiamo init base
    StateBasedEpEnergyConsumer::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        rxPowerConsumptionSignal = registerSignal("rxPowerConsumption");
        txPowerConsumptionSignal = registerSignal("txPowerConsumption");
        sleepTimeSignal = registerSignal("sleepTime");
        sleepStateChangedSignal= registerSignal("sleepStateChanged");
        previousRadioMode = radio->getRadioMode(); //mi serve per capire lo stato della radio precedente al nuovo stato
        sonnellini=0;
        WATCH(powerConsumptionRx);
        WATCH(powerConsumptionTx);
    }
}

void MY_StateBasedEpEnergyConsumer::receiveSignal(cComponent *source,
        simsignal_t signal, intval_t value, cObject *details)
{
    // usiamo il comportamento della classe base per il totale
    StateBasedEpEnergyConsumer::receiveSignal(source, signal, value, details);

    // Solo nei segnali rilevanti, facciamo il dettaglio
    if (signal == IRadio::radioModeChangedSignal
         || signal == IRadio::receptionStateChangedSignal
         || signal == IRadio::transmissionStateChangedSignal
         || signal == IRadio::receivedSignalPartChangedSignal
         || signal == IRadio::transmittedSignalPartChangedSignal) {

        auto newMode = (IRadio::RadioMode)value;
                 if (newMode == IRadio::RADIO_MODE_SLEEP && previousRadioMode != IRadio::RADIO_MODE_SLEEP) {
                     sleepStartTime = simTime(); //inizio a contare i secondi di sleep
                     emit(sleepStateChangedSignal, true); //emetto segnale che mi dice che nodo ha iniziato a dormire
                 }
                 else if (previousRadioMode == IRadio::RADIO_MODE_SLEEP && newMode != IRadio::RADIO_MODE_SLEEP) {
                     simtime_t sleepDuration = simTime() - sleepStartTime; //vuol dire che ho appena finito di dormire quindi calcolo totale
                     sleepTotalTime += sleepDuration; //mi salvo in una variabile la somma di tutti gli sleep fatti
                     sonnellini++;
                     //recordScalar("totalSleepTime", sleepTotalTime); SPOSTATO NELLA FUNZIONE FINISH
                     emit(sleepTimeSignal, sleepTotalTime.dbl()); // sleepTimeSignal registrato in initialize()
                     emit(sleepStateChangedSignal, false);
                 }
                 previousRadioMode = newMode;

        auto arr = computePowerConsumptionDetailed();
        // arr[0] è totale, arr[1] RX, arr[2] TX
        powerConsumption = arr[0]; // eredita la variabile protected del base
        powerConsumptionRx = arr[1];
        powerConsumptionTx = arr[2];


        // emetti i segnali aggiuntivi
        emit(rxPowerConsumptionSignal, powerConsumptionRx.get());
        emit(txPowerConsumptionSignal, powerConsumptionTx.get());


        EV << "t=" << simTime()
                << "s | Node=" << getParentModule()->getFullName()
                << " | TotalPower=" << powerConsumption
                << " | RX=" << powerConsumptionRx
                << " | TX=" << powerConsumptionTx
                << " | RadioMode=" << newMode
                << endl;
    }



}


std::array<W, 3> MY_StateBasedEpEnergyConsumer::computePowerConsumptionDetailed() const {
    IRadio::RadioMode radioMode = radio->getRadioMode();
    /*Andiamo a controllare la tipologia di comunicazione che sta effettuando la radio*/

    if (radioMode == IRadio::RADIO_MODE_OFF) //radio spenta
        return {offPowerConsumption, W(0), W(0)};
    else if (radioMode == IRadio::RADIO_MODE_SLEEP) //radio in sleep
        return {sleepPowerConsumption, W(0), W(0)};
    else if (radioMode == IRadio::RADIO_MODE_SWITCHING) //radio sta cambiando comunicazione
        return {switchingPowerConsumption, W(0), W(0)};

    /*Mi inizializzo non più solo powerConsumption ma anche altre due variabili per tenere traccia del consumo inerente alla ricezione
     * e trasmissione dei pacchetti ed altro*/
    W powerConsumption = W(0);
    W powerConsumptionRx = W(0);
    W powerConsumptionTx = W(0);

    /*Una volta che sono sicuro che la mia radio non sia ne spenta ne in switching ne in sleep allora sicuramente o sto ricevendo o
     * sto trasmettendo o sto facendo entrambe le cose*/
    IRadio::ReceptionState receptionState = radio->getReceptionState();
    IRadio::TransmissionState transmissionState = radio->getTransmissionState();


    /* --------RICEZIONE-------------
     * Abbiamo diversi momenti di ricezione, in particolare abbiamo un consumo differente se la radio è in ascolto
     * sul canale ancora libero (IDLE), quando nota che gli si sta trasmettendo qualcosa (BUSY) e quando effettivamente riceve qualcosa
     * da decodificare (RECEIVING)*/

    if (radioMode == IRadio::RADIO_MODE_RECEIVER
            || radioMode == IRadio::RADIO_MODE_TRANSCEIVER) {
        switch (receptionState) {
        case IRadio::RECEPTION_STATE_IDLE:
            powerConsumption += receiverIdlePowerConsumption;
            break;
        case IRadio::RECEPTION_STATE_BUSY:
            powerConsumption += receiverBusyPowerConsumption;
            break;
            /*In questo ultimo caso entrano in gioco le variabili nuove, in quanto le vado a popolare con effettivamente
             * i valori di consumo inerenti a questa fase di comunicazione*/
        case IRadio::RECEPTION_STATE_RECEIVING: {
            auto part = radio->getReceivedSignalPart();
            /*Nel caso che effettivamente stiamo ricevendo pacchetto è importante anche sapere quale parte del pacchetto
             * poichè vi è un diverso consumo in base ad esso.*/
            switch (part) {
            case IRadioSignal::SIGNAL_PART_NONE:
                break;
            case IRadioSignal::SIGNAL_PART_WHOLE:
                powerConsumptionRx += receiverReceivingDataPowerConsumption;
                break;
            case IRadioSignal::SIGNAL_PART_PREAMBLE:
                powerConsumptionRx += receiverReceivingPreamblePowerConsumption;
                break;
            case IRadioSignal::SIGNAL_PART_HEADER:
                powerConsumptionRx += receiverReceivingHeaderPowerConsumption;
                break;
            case IRadioSignal::SIGNAL_PART_DATA:
                powerConsumptionRx += receiverReceivingDataPowerConsumption;
                break;
            default:
                throw cRuntimeError("Unknown received signal part");
            }
            break;
        }
        case IRadio::RECEPTION_STATE_UNDEFINED:
            break;
        default:
            throw cRuntimeError("Unknown radio reception state");
        }
    }
    /*Lo stesso identico ragionamento lo si applica sulla trasmittente*/
    if (radioMode == IRadio::RADIO_MODE_TRANSMITTER
            || radioMode == IRadio::RADIO_MODE_TRANSCEIVER) {
        switch (transmissionState) {
        case IRadio::TRANSMISSION_STATE_IDLE:
            powerConsumption += transmitterIdlePowerConsumption;
            break;

        case IRadio::TRANSMISSION_STATE_TRANSMITTING: {
            auto part = radio->getTransmittedSignalPart();
            switch (part) {
            case IRadioSignal::SIGNAL_PART_NONE:
                break;
            case IRadioSignal::SIGNAL_PART_WHOLE:
                powerConsumptionTx += transmitterTransmittingPowerConsumption;
                break;
            case IRadioSignal::SIGNAL_PART_PREAMBLE:
                powerConsumptionTx +=
                        transmitterTransmittingPreamblePowerConsumption;
                break;
            case IRadioSignal::SIGNAL_PART_HEADER:
                powerConsumptionTx +=
                        transmitterTransmittingHeaderPowerConsumption;
                break;
            case IRadioSignal::SIGNAL_PART_DATA:
                powerConsumptionTx +=
                        transmitterTransmittingDataPowerConsumption;
                break;
            default:
                throw cRuntimeError("Unknown transmitted signal part");
            }
            break;
        }
        case IRadio::TRANSMISSION_STATE_UNDEFINED:
            break;
        default:
            throw cRuntimeError("Unknown radio transmission state");
        }
    }
    return {powerConsumption+powerConsumptionRx+powerConsumptionTx, powerConsumptionRx, powerConsumptionTx};
}

/*Funzione standard dei moduli cModule che prevede l'esecuzione di procedure/funzioni definite all'interno di essa al momento
 *del termine della simulazione, in questo caso andiamo a registrare anche lo scalare del tempo di sleep totale per ottenere non solo
 *del vettoriale ma anche scalare.*/
void MY_StateBasedEpEnergyConsumer::finish()
{
    // Registra lo scalare solo una volta alla fine
    recordScalar("totalSleepTime", sleepTotalTime);
    recordScalar("Sonnellini", sonnellini);
    // Se vuoi, emetti anche i segnali finali
    emit(sleepTimeSignal, sleepTotalTime.dbl());
}

} // namespace physicallayer

} // namespace inet

