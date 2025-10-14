# Simulazione di Attacchi Denial of Sleep su Reti di Sensori Wireless

## Descrizione del progetto

Tesi triennale in Informatica avente come obiettivo la **simulazione di attacchi su reti WSN (Wireless Sensor Networks)** a topologia astratta (clustering).

### Obiettivo

L'obiettivo del progetto è analizzare l’impatto energetico di diverse tipologie di attacco **Denial of Sleep**, realizzate tramite flooding di datagrammi UDP, su una rete di sensori wireless. La simulazione è stata realizzata utilizzando **OMNeT++**, **INET** e moduli personalizzati sviluppati estendendo componenti del framework INET.

---

## Topologia di rete

La morfologia della rete prevede **7 dispositivi**:

* **4 × MY_SENSOR_NODE** (nodi sensore)
* **1 × GATEWAY** (WirelessHost)
* **1 × SERVER** (StandardHost)
* **1 × MALICIOUS_NODE** (StandardHost)

---

## Scenari di simulazione

Sono stati progettati diversi scenari, ognuno rappresentante un diverso comportamento del nodo malevolo:

* **Safe** → scenario di baseline senza attacco, utile per il confronto con gli scenari successivi.
* **Attacco_Aggressivo** → il nodo malevolo trasmette datagrammi UDP ad alta frequenza e di notevole dimensione in modo continuo.
* **Attacco_Silenzioso** → l’attaccante utilizza funzioni probabilistiche per variare frequenza e dimensione dei pacchetti, evitando un comportamento costante e facilmente rilevabile.
* **Attacco_Burst** → scenario intermedio, con alternanza di fasi silenziose e fasi di trasmissione intensa a frequenza maggiore dell’attacco aggressivo.

---

## Descrizione dei moduli personalizzati

Il modulo **MY_SENSOR_NODE** estende il `SensorNode` di INET includendo ulteriori componenti funzionali non presenti nella versione originale.

### 1. Energy Consumer personalizzato

Il modulo `SensorStateBasedEpEnergyConsumer` di base computa unicamente l’energia totale consumata, senza distinguere tra trasmissione e ricezione. È stato quindi sviluppato un **modulo personalizzato** in grado di:

* differenziare il consumo energetico tra **TX** e **RX**;
* monitorare il **tempo trascorso in modalità sleep** e il **numero di risvegli** del nodo.

Queste metriche risultano particolarmente utili per scenari che prevedono protocolli MAC con gestione dello stato sleep.

### 2. Energy Storage

È stato impiegato un modulo `SimpleEpEnergyStorage`, che consente di impostare l’energia nominale e iniziale del nodo, a differenza dell’`IdealEpEnergyStorage` che non prevede scaricamento della batteria.

### 3. Energy Management

Modulo necessario per gestire lo **spegnimento del nodo** al raggiungimento di una soglia minima di energia residua. Per abilitare tale funzionalità è stato necessario attivare il parametro `hasStatus` sul nodo.

### 4. Interfaccia radio

È stata utilizzata una **IEEE 802.15.4 Narrowband Interface** (`Ieee802154NarrowbandInterface`), conforme agli standard IEEE, per la gestione della comunicazione radio dei nodi.

---

## Routing

Per minimizzare il consumo energetico dovuto alla configurazione e gestione del routing, è stato impiegato un **routing statico**. In questo modo i nodi non devono eseguire procedure di autoconfigurazione o inviare pacchetti di controllo aggiuntivi.

---

## File di configurazione

Sono presenti due file principali di configurazione `.ini`, uno per ciascun protocollo MAC utilizzato:

* **B-MAC**
* **IEEE 802.15.4 (CSMA/CA)**

I risultati vengono salvati automaticamente in directory distinte in base allo scenario di attacco, con un file `.anf` (es. `General.anf`) per la visualizzazione e l’analisi comparativa dei risultati.

---

## Installazione e avvio

1. Installare **OMNeT++** (versione compatibile con INET 4.x o superiore).
2. Clonare o importare il progetto nella **workspace di OMNeT++**.
3. Installare e importare il framework **INET**.
4. Aggiungere **INET** alle dipendenze del progetto, se necessario.
5. Compilare il progetto tramite IDE di OMNeT++.
6. Avviare le simulazioni selezionando lo scenario desiderato dal file `.ini` corrispondente.

---

## Note finali

Il progetto fornisce un ambiente di simulazione utile per analizzare e confrontare il comportamento energetico di reti di sensori wireless sotto diverse tipologie di attacco Denial of Sleep.

I risultati ottenuti consentono di osservare differenze significative nei consumi energetici (TX, RX, e sleep time) e di valutare l’efficacia dei protocolli MAC nel mitigare l’impatto degli attacchi.
