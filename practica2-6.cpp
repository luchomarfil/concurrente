


int tareasPendientes = T; //cantidad de tareas a realizar
int cantMaxTareas = 0;    //maximo del/los mejor/es empleado/s
int finalizaron = 0;      //cantidad de empleados que terminaron la jornada
Sem barrera = 0;          //barrera para esperar a que todos terminen y recibir el premio
Sem finalizacion = 1;     //exclusion mutua para variable que contabiliza los que van terminando
Sem tomarTarea = 1;       //ExMu para los que van a tomar una tarea
Sem calcularMaximo = 1;   //ExMu para los que estan actualizando el maximo

Process Empleado(id=1..E){
   int cantidadRealizadas = 0;    //cada empleado contabiliza sus tareas realizadas
   P(tomarTarea);                 //pide leer las tareas pendientes
   while(tareasPendientes>0){
     //tomo la tarea de tareasPendientes
     tareasPendientes--;
     V(tomarTarea);               //libera lectura de tareas pendientes
     delay(random);               //hago la tarea
     P(calcularMaximo);           //pide calcular el maximo de tareas. Se usa otro semaforo
                                  //para maximizar concurrencia con los que estan tomando tareas
     realizadas++;
     if(realizadas>cantMaxTareas){
       cantMaxTareas = realizadas;
     }
     V(calcularMaximo);           //libera calculo maximo tareas
     P(tomarTarea);               //pide leer las tareas pendientes
   }
   V(tomarTarea); //libera la exclusion mutua cuando sale del while
   //Aca comienza la barrera. Cada proceso evalua si es el ultimo despierta
   //a todos los procesos para recibir el premio, sino se duerme a esperar al
   //ultimo
   P(finalizacion);               //pide bloqueo para actualizar la cantidad que finalizaron
   finalizaron++;
   if(finalizaron==E){            //si es el ultimo despierta a todos
      for (i=1 to E){
          V(barrera);             //despierto a todos los procesos dormidos que son E
      }
   }
   V(finalizacion);               //libero lectura
   P(barrera);                    //duermo hasta que el ultimo despierte a todos

   //Ahora todos pueden saber si fueron premiados. Es premiado quien
   //tiene cantidad realizadas == a cantMaxTareas
   if(realizadas==cantMaxTareas){
     recibirPremio();
   }

}
