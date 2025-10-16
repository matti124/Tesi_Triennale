
import inet.physicallayer.wireless.common.energyconsumer.StateBasedEpEnergyConsumer;
simple MY_StateBasedEpEnergyConsumer extends StateBasedEpEnergyConsumer
{
    parameters:
        @class(MY_StateBasedEpEnergyConsumer);
	
	//Segnale per raccogliere statistica vettoriale e scalare (media) dell'energia consumata in Ricezione dal nodo
        @signal[rxPowerConsumption](type=double);
        @statistic[rxPowerConsumptionVector](title="RX Power consumption over time"; source=rxPowerConsumption; record=vector; interpolationmode=sample-hold; unit=W);
        @statistic[rxPowerConsumptionScalar](title="RX Power consumption stats"; source=rxPowerConsumption; record=mean; unit=W);

	//Segnale per raccogliere statistica vettoriale e scalare (media) dell'energia consumata in Trasmissione dal nodo
        @signal[txPowerConsumption](type=double);
        @statistic[txPowerConsumptionVector](title="TX Power consumption over time"; source=txPowerConsumption; record=vector; interpolationmode=sample-hold; unit=W);
        @statistic[txPowerConsumptionScalar](title="TX Power consumption stats"; source=txPowerConsumption; record=mean; unit=W);

	//Segnale per raccogliere statistica vettoriale dell'andamento dello sleep Time del nodo nel tempo
        @signal[sleepTime](type=double);
        @statistic[sleepTimeVector](title="Total sleep time over time"; source=sleepTime; record=vector; interpolationmode=sample-hold; unit=s);
}

