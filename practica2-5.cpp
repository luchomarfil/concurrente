Sem   colaDisponible = 1; //semaforo para controlar exclusion mutua para acceso a la cola
Queue cola;
Sem   timer[N] = {0} //inicializados en cero
Sem   consultarAyudante = 0;
Sem   esperaConsulta[N] = {0};
Sem   datosGenerales = 1; // Semaforo de exlusion mutua para variable compartida por todos los procesos y todos los ayudantes
Sem   datos[N] = {1}; //Semaforo para proteger interferencias entre Ayudantes y Timers para un Alumno
int evento[N] = 0; // arreglo de estados
bool correcto[N]=false; //arreglo para el resultado del ejercicio de la practica
int cantAlumnos = N;


process Alumno(id=1..N){
    delay(Random); //hace la tarea el alumno
    P(colaDisponible);
    cola.push(id);
    V(timer[id]);
    V(colaDisponible);
    V(consultarAyudante);
    P(esperaConsulta[id]);
    //Se duerme hasta que lo despierte o el ayudante para corregir, o lo despierte
    // el timer para avisarle que se le acabo el tiempo
    if(evento[id]0==1){ //esto es si lo desperto el timer
            //se retira del aula
            P(datosGenerales);
            cantAlumnos--;
            V(datosGenerales);
            V(datos[id]);
    }
    else{
        while(!correcto[id}){
            V(datos[id]); // deja de usar los datos que le puede pasar el ayudante que corrigio

            delay(Random); //corrige la tarea
            P(colaDisponible); //pide bloqueo sobre la cola
            cola.push(id);
            V(colaDisponible);
            V(consultarAyudante);

            P(esperaConsulta[id]); // le avisa a los ayudante que esta nuevamente la tarea disponible
        }
        //se retira del aula
        P(datosGenerales); //pide exclusion mutua para modificar la cantidad de alumnos
        cantAlumnos--;
        if(cantAlumnos==0){ // si es el ultimo alumno, despierta a todos los ayudantes para que se vayan
          for i=1 to A{
            V(consultarAyudante);
          }

        }
        V(datosGenerales); //libera el bloqueo sobre las variables compartidas

}


}

process Timer(id=1..N){
    P(timer[id]); // se duerme a esperar que el alumno termine la tarea y se ponga en la cola
    delay(1000 * 60 * 15); // 15 minutos periodo de espera
    P(datos[id]); // bloquea la informacion sobre el proceso para evitar que el ayudante comience a corregirlo
    if(evento[id]!=2) // si no fue corregido ya por el ayudante
    {
      evento[id]=1; //marca que el proceso deberia abandonar la cola
      V(esperaConsulta[id]); //despierta al proceso para que lea el resultado
    }
}


process Ayudante(id=1..A){
    int idProcesoActual;
    P(consultarAyudante); //espera hasta que lo despierte un alumno
    P(datosGenerales); //cuando lo despierta bloquea la variable cantAlumnos para ver si queda alguno en la habitacion
    While(cantAlumnos>0){
      V(datosGenerales); //libero la variable compartida porque ya la lei
      P(colaDisponible); //espero acceso exclusivo a la cola
      cola.pop(idProcesoActual);
      V(colaDisponible); //libero la cola

      P(datos[id]); // bloqueo sobre id para evitar interferencias con el timer
      correcto[idProcesoActual] = corregir();
      evento[idProcesoActual] = 2; //marca como estado que el ayudante atendio el alumno
      V(esperaConsulta[idProcesoActual]); //despierta al alumno para que busque el resultado
      P(consultarAyudante); // se duerme el ayudante, hasta q el alumno lo vuelva a despertar
      P(datosGenerales);    // cuando se despierta pide bloqueo para leer la cantidad de alumnos sin interferencias
    }

}
