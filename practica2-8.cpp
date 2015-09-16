Sem colaDisponible = 1; //exclusion mutua para acceder a la cola
Queue cola;             //cola que contiene los procesos camion en orden de llegada
Sem timer[N]  = 0;      //Semaforo de condicion para iniciar los timers (arranca procesos dormidos)
Sem espera[N] = 0;      //Semaforo para que cada camion espere o el timer u otro camion
Sem datos[N]  = 1;      //Semaforo para compartir datos de forma excluyente entre Camion que despierta y Timer
int estadoCamion[N] = {0}; //Estado interno del camion, se refleja si el camion fue despertado por un
                           //Camion o por el timer. 1=Abandona por timer, 2=Descarga cereal, 0=no Seteado


Process Camion(id=1..N){
    int idSiguiente = 0; //cada camion sabe a quien va a despertar
    P(colaDisponible); //pide acceso por exclusion mutua a la cola
    if(!empty(cola)){  //si no es el primer camion en llegar
        cola.push(id); //se agrega a la lista de espera
        V(colaDisponible); //libera el acceso a la cola
        V(timer[id]); //inicia el timer de 2hs
        P(espera[id]); //se duerme por condicion hasta que lo despierte o un camion o un timer
    }
    else{ //si es el primer camión en llegar, pasa derecho.
      V(colaDisponible); //Libera la cola para que se puedan seguir encolando camiones
      P(datos[id]); //Bloquea para evitar interferencias con el timer, va a leer datos del estado
    }

    //todos los camiones, abandonen o no, hacen el mismo proceso, la diferencia
    //es que los que no abandonan, descargan la mercaderia.
    //pero si un proceso camion fue despertado por timer y abandona, de todas
    //maneras, tiene que sacar al siguiente de la cola y despertarlo, puede que ese otro
    //no este esperando por mas de 2hs.
    if(estadoCamion[id]!=1){  //si no lo desperto el timer, puede ser el primero o despertado por otro
                              //0, es el primero; 2=despertado por otro
          V(datos[id]); //libera la exclusion mutua entre timer y otro camion que lo despierte
          delay(Random);  //descarga la mercaderia
    }

    P(colaDisponible); //pide acceso a la cola para despertar a otro camion
    if(!empty(cola)){ //si hay otro camion en la cola
        cola.pop(idSiguiente); //saca el camion de la cola para saber su process id
        P(datos[idSiguiente]); //pide exclusion mutua para intercambiar datos con el camion
                               //sin interferencias del timer
        V(colaDisponible);     //libera la cola, para que sigan llegando camiones o la pueda usar algun timer
        if(estado[idSiguiente]==0){ //si todavia el timer no la seteo
          estado[idSiguiente]=2; //setea el estado Descarga Cereal
        }
        V(espera[idSiguiente]);//despierta al siguiente camion
    }
    else{//si no quedan camiones
        V(colaDisponible);  //libero la exclusion mutua sobre la cola
    }



}

Process Timer(id=1..N){
  P(timer[id]); //duerme el proceso hasta que la llegada de un camion lo despierte
  delay(1000*60*60*2); //espera de 2hs para avisar al camion de abandonar
  P(datos[id]); //bloquea con exclusion mutua para evitar interferencia con otro
                //camion que quiera modificar estado de su anterior camion
  if(estado[id]==0){ //si el estado aun no fue seteado por su camion anterior
    estado[id]=1; //le indica que no debe descargar cereal
  }
  V(espera[id]); //despierta al camion
  //el camion es quien va a levantar la exclusion de P(datos[id]), de esta maneras
  //se evita que antes que despierte el camion, lo despierte otro camion.
  //Entonces el camion despertado por el timer, libera la exclusion, luego de leer
  //el estado seteado por el timer, y se asegura de q la info no fue pisada por otro camion
}
