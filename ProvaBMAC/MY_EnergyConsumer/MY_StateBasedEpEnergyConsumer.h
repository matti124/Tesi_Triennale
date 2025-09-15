//
// ExtendedStateBasedEpEnergyConsumer.h
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Questa classe estende StateBasedEpEnergyConsumer per fornire
// una misura dettagliata del consumo energetico separando RX e TX.
//

#pragma once

#include "inet/physicallayer/wireless/common/energyconsumer/StateBasedEpEnergyConsumer.h"

namespace inet {
namespace physicallayer {

class INET_API MY_StateBasedEpEnergyConsumer : public StateBasedEpEnergyConsumer
{
  protected:
    // Variabili aggiuntive per il tracciamento dettagliato
    W powerConsumptionRx = W(0);
    W powerConsumptionTx = W(0);

    // Segnali aggiuntivi
    simsignal_t rxPowerConsumptionSignal;
    simsignal_t txPowerConsumptionSignal;

  protected:
    /** Inizializza i nuovi segnali e watchers */
    virtual void initialize(int stage) override;

    /** Ridefinisce il receiveSignal per emettere anche RX/TX */
    virtual void receiveSignal(cComponent *source, simsignal_t signal, intval_t value, cObject *details) override;

    /** Nuovo metodo per il calcolo dettagliato (totale, RX, TX) */
    virtual std::array<W, 3> computePowerConsumptionDetailed() const;
};

} // namespace physicallayer
} // namespace inet
