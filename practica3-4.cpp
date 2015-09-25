process Cliente(id=1..N){
    String miEstado;
    mTimer[id].iniciar(); //destraba el timer para que inicie
    mBanco.encolarCliente(id);
    //en este momento he salido de la cola. O  porque me marcaron como
    //atendido o porque el timer me marco como abandonado.
    //averiguo mi estado
    mBanco.estado(id,miEstado);
    if(miEstado=="ATENDIDO"){
      //hago la tarea que vine a hacer al banco
      delay(random);
      //deberia avisarle al empleado cuando termina que se va con un
      //metodo miBanco.irse(id)??
    }
    else{
      //se va mascando bronca y puteando contra el capitalismo
    }
}

process Timer(id=1..N){
  mTimer[id].esperar();
  delay(1000*60*15);
  mBanco.abandonar(id);
}

process Empleado(id=1..2){
  while(bancoEsteAbierto){
    mBanco.atender();
  }
}

Monitor mTimer(id=1..N){
  cond e;
  bool llegue = false;

  procedure iniciar(){
    if(!llego){ //solo duermo al cliente si el timer no llego primero
      wait(e);
    }
  }
  procedure esperar(){
    llegue = true; //se marca que llego el timer, para en el iniciar decidir si dormir
                   //o no al cliente
    signal(e);
  }
}

Monitor mBanco{
  int esperandoEmpleado = 0;
  int esperandoCliente = 0;
  queue colaClientes;
  cond empleado;
  cond cliente;
  String estadoCliente[] = {""}

  procedure encolarCliente(id){
    esperandoEmpleado++; //se marca que hay un cliente esperando empleado
    colaClientes.push(id); //se agrega en la cola de clientes
    //si hay un empleado esperando clientes, se despierta el primer empleado de la cola
    //condicion
    if(esperandoCliente > 0){
      esperandoCliente--;
      signal(empleado);
    }
    //me duermo a esperar que me toque mi turno en la cola
    wait(cliente);
  }

  procedure atender(){
    int idActual;
    //si no hay clientes esperando por un empleado, el empleado se duerme
    if(esperandoEmpleado==0){
      esperandoCliente++;
      wait(empleado);
    }
    //aca ya estoy atendiendo a un cliente
    esperandoEmpleado--; //hay un cliente menos esperando por empleado
    colaClientes.pop(idActual); //recupero el id cliente de la cola para setearle el estado
    if(estadoCliente[idActual]==''){ //si todavia no esta seteado el esado
      estadoCliente[idActual] = "ATENDIDO";
    }
    signal(cliente); //despierto al cliente para que sepa que yo lo atendi
  }

  //recupera el estado para el idCliente dado y lo retorna en la variable estado
  procedure estado(id, var estado){
      estado = estadoCliente[id];
  }

  procedure abandonar(id){
      if(estado[id]==""){
        estado[id] = "ABANDONO";
      }
  }
}
