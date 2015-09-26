process Operario(id=1..N){
  int nroGrupo;
  bool sePuede;
  //consulta al monitor fabrica para obtener el numero de grupo
  mFabrica.obtenerGrupo(id,nroGrupo);
  //aca ya tengo el numero de grupo y me reporto al grupo para esperar al equipo
  mGrupo[nroGrupo].esperarEquipo(id);
  //cuando se reune todo el equipo, el mFabrica despierta a todos los procesos
  //ahora cada integrante del grupo le avisa a su monitor de grupo
  //que esta listo para fabricar. El monitor de grupo esperara que todos los del
  //grupo esten listos, ahi, intentara obtener el permiso de fabricar una camisa
  //si no supera ya la cantidad a 5000. Eso se retorna en sePuede. si se puede ya
  //se reservo la fabricacion de una camisa
  mGrupo[nroGrupo].fabricar(sePuede);
  while(sePuede){ //fabrico una camisa
      delay(random);  //delay cada empleado consige sus elementos que tiene a cargo
      mGrupo[nroGrupo].conseguirElementos(2); //cada empleado aporta 2 elementos y espera a que esten todos los elementos
      //una vez que se consiguen los elementos cada uno hace su parte en la fabricacion
      delay(random);
      //le avisa al monitor de su grupo que la camisa ya esta ensamblada. Cuando todos
      //los integrantes le avisen al monitor que la camisa ya esta ensamblada, pueden seguir adelante
      //puede pasar que un operario se atrase en su parte de la tarea de ensamble
      mGrupo[nroGrupo].esperarEquipo(id);
      //en este momento, nuevamente todos los empleados le piden al grupo fabricar
      //otra camisa, como antes del while
      mGrupo[nroGrupo].fabricar(sePuede);
  }
  //aca ya no se pueden fabricar mas camisas, lo que se hace ahora es esperar a todos los operarios
  //le avisamos a otro monitor que nos duerma hasta que todos los operarios hayan llegado y asi poder
  //retirarse todos
  mSalida.esperarSalida(id);
}

process Encargado(){
    int ids[] = 0; //es un arreglo de ids de empleados
    int recorridos = 0;
    int nroGrupo = 0;
    mFabrica.listaEmpleados(ids); //le solicito a la fabrica la lista de empleados q me la ponga en el array
    while(recorridos < ids.length){
      nroGrupo++;
      int idsGrupo[] = 0;
      for (i=0 to 3){ //recorro cuatro posiciones del arreglo y armo el arreglo de grupo
        idsGrupo[i] = ids[recorridos];
        recorridos++;
      }
      //le informo a la fabrica que ya arme un grupo y sigo con el siguiente while
      mFabrica.informarGrupo(nroGrupo,idsGrupo);
    }
    //ahora ya no tengo que hacer mas nada como encargado
}

Monitor Fabrica(){
  //recibe el numero de proceso y retorna el numero de grupo asignado
  cond esperandoAsignacion[]; //arreglo de condicion privado, cada empleado se duerme hasta tener grupo
  cond encargado;
  int empleados[]; //representa a los empleados que van llegando y que espera el encargado
  int grupoAsignado[] = 0; //cada indice es el proceso, y el contenido es el numero de grupo
  int cantidadLlegaron = 0;
  bool esperandoEmpleados = false;
  int camisasFabricadas = 0;
  procedure obtenerGrupo(int id, out int nroGrupo){
      //se agrega a la lista de empleados
      empleados.[cantidadLlegaron]=id;
      cantidadLlegaron ++;
      //si llegaron todos y esta esperando el encargado, le aviso. sino esta esperando el encargado,
      //cuando pida la lista, no se dormira si ya llegaron todos los empleados
      if(esperandoEmpleados && cantidadLlegaron == X){
        signal(encargado);
      }
      //se duerme a esperar que le asignen un grupo
      wait(esperandoAsignacion[id]);
      //aca ya tiene un numero de grupo asignado, lo busca y lo retorna
      nroGrupo = grupoAsignado[id];
  }

  procedure listaEmpleados(out int listaEmpleados[]){
      //si todavia no llegaron todos, espero a que el
      //ultimo en llegar me haga un signal en encargado
      if(cantidadLlegaron!=X){
        wait(encargado)
      }
      //aca llegaron todos, retorno en listaEmpleados la lista de empleados
      listaEmpleados = empleados;
  }

  //tengo que despertar a todos los empleados que durmieron en condicion privada y setearles antes
  //su numero de grupo en el arreglo de numero de grupos
  procedure informarGrupo(nroGrupo,idsGrupo){
    //recorro la lista de ids
    for(id=0; id < idsGrupo.length;i++){
      //le digo que grupo le corresponde
      grupoAsignado[id]  = nroGrupo;
      //lo despierto para que pueda reunirse con sus compañeros
      signal(esperandoAsignacion[id]);
    }
  }

  //el monitor decide si autoriza o no la creacion, si la autoriza
  //ya reserva su fabricacion. Pese a un no estar fabricada
  procedure fabricar(out bool sePuede){
    if(camisasFabricadas==5000){
      sePuede = false;
    }
    else{
      camisasFabricadas++;
      sePuede = true;
    }
  }
}

Monitor mGrupo(id=1..X/4){
    int cantidadLlegaron = 0;
    cond esperando;
    cond esperandoElementos;
    bool sePuedeFabricar = false; //esta es global al monitor para que todos los empleados se enteren
                                  //que respondio la fabrica. Sino se entera solo el que hace el llamado a
                                  //monitor de fabrica
    int elementosConseguidos =0;
    //cada empleado que ya sabe su grupo se comunica con su monitor
    //para que dormirse hasta que lleguen los cuatro
    procedure esperarEquipo(id){
      cantidadLlegaron++;
      if(cantidadLlegaron==4){
        cantidadLlegaron = 0;
        signalAll(esperando);
      }
      else{
        wait(esperando);
      }

    }


    procedure fabricar(out bool sePuede){
      cantidadLlegaron++;
      if(cantidadLlegaron==4){
        cantidadLlegaron = 0;
        //el ultimo le pide a la fabrica si puede generar otra camisa, para evitar
        //cuatro pedidos. La fabrica ya devuelve en el parametro out
        mFabrica.fabricar(sePuedeFabricar);
        signalAll(esperando);
      }
      else{
        wait(esperando);
      }
      //aca todos acceden a si se puede o no fabricar, tener en cuenta que si fue
      //el cuarto no se durmio. sino los demas despues del wait acceden al valor
      sePuede = sePuedeFabricar;

    }

    //barrera que duerme a los empleados hasta que se hayan recolectado 8 elementos
    procedure conseguirElementos(int conseguidosEmpleado){
        elementosConseguidos += conseguidosEmpleado;
        if(elementosConseguidos<8){
           wait(esperandoElementos);
        }
        else{
           elementosConseguidos = 0; //reseteo para la proxima fabricacion
           signalAll(esperandoElementos);
        }
    }


}

Monitor mSalida{
    cond esperarSalida;
    int cantidadLlegaron = 0;
    procedure esperarSalida(id){
        cantidadLlegaron++;
        if(cantidadLlegaron<X){
          wait(esperarSalida);
        }
        else{
          signalAll(esperarSalida);
        }
    }

}
