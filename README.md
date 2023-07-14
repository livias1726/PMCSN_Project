# MODIFICHE MASSIVE

* Gestione delle politiche di scheduling come flag di compilazione (ABO_COMP, ABO_ID)
  * Gestione del matching in modo analogo.
* Trigger del matching durante la gestione dell'arrivo di un organo
  * Se è in coda un paziente compatibile e allocabile (active), allora l'organo viene dispatchato subito.
* Trigger del matching durante la gestione dell'arrivo di un paziente
  * Se il paziente è in cima alla lista per le condizioni di arrivo, relativamente al suo gruppo sanguigno, 
  allora viene servito subito.
* Modifica della segnatura delle funzioni di arrivo per gestire il matching all'interno.
* Modifica delle funzioni degli eventi in relazione alla rimozione degli switch.