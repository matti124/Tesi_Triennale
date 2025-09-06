import inet.node.inet.*;
import inet.networklayer.configurator.ipv4.*;
import inet.physicallayer.wireless.apsk.packetlevel.*;
import inet.node.ethernet.Eth100M;
import inet.visualizer.common.IntegratedVisualizer;

network SensorNetwork
{
    submodules:
        visualizer: IntegratedVisualizer;
        radioMedium: ApskScalarRadioMedium;
        Configuratore: Ipv4NetworkConfigurator;
        IoTSensor1: SensorNode {
            @display("i=misc/sensor2;p=99,139");
        }

        IoTSensor2: SensorNode {
            @display("i=misc/sensor2;p=210,100");
        }
        IoTSensor3: SensorNode {
            @display("i=misc/sensor2;p=384,156");
        }

        IoTGateway: SensorNode {
            @display("i=misc/sensor;p=101,262");
        }

        server: StandardHost {

            @display("p=255,248");
        }
    connections:
        IoTGateway.ethg++ <--> Eth100M <--> server.ethg++;



}
