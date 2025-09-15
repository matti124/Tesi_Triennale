import inet.physicallayer.wireless.common.energyconsumer.StateBasedEpEnergyConsumer;
simple MY_StateBasedEpEnergyConsumer extends StateBasedEpEnergyConsumer
{
    parameters:
        @class(MY_StateBasedEpEnergyConsumer);
        @signal[rxPowerConsumption](type=double);
        @statistic[rxPowerConsumption](title="RX Power consumption"; source=rxPowerConsumption; record=vector; interpolationmode=sample-hold; unit=W);

        @signal[txPowerConsumption](type=double);
        @statistic[txPowerConsumption](title="TX Power consumption"; source=txPowerConsumption; record=vector; interpolationmode=sample-hold; unit=W);

}
