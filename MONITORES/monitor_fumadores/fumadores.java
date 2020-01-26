import java.util.Random;
import monitor.*;

class Estanco extends AbstractMonitor{
    private int ing_anterior;
    private Condition sem_estanquero = makeCondition();
    private Condition[] sem_fumador = new Condition[3];

    public Estanco(){
        this.ing_anterior = -1;
        for (int i=0; i<3; i++)
            sem_fumador[i] = makeCondition();
    }
    // Invocado por cada fumador, indicando su ingrediente o numero
    public void obtenerIngrediente(int miIngrediente){
        enter();
            if (miIngrediente != ing_anterior){
                sem_fumador[miIngrediente].await();
            }
            System.out.println("Fumador" + miIngrediente +" ,recoge el ingrediente & fuma... " );
            // El fumador recoge el ingrediente (pone -1)
            ing_anterior = -1;
            sem_estanquero.signal();
        leave();
    }
    // Invocado por el estanquero, indicando el ingrediente que pone
    public void ponerIngrediente(int ingrediente){
        enter();
            ing_anterior = ingrediente;
			if(ingrediente==0)System.out.println("Estanquero, repartiendo papel" );
			if(ingrediente==1)System.out.println("Estanquero, repartiendo cerillas" );
			if(ingrediente==2)System.out.println("Estanquero, repartiendo tabaco" );
            
            sem_fumador[ingrediente].signal();
        leave();
    }
    // Invocado por el estanquero
    public void esperarRecogidaIngrediente(){
        enter();
            if (ing_anterior != -1)
                sem_estanquero.await();
        leave();
    }
    
	public int recuperarValor(){
		
		return ing_anterior;
		
	
	}
}


class Estanquero implements Runnable{

    private Estanco estanco;
    public Thread thr;

    public Estanquero(Estanco pMonEstanco){

        estanco    = pMonEstanco;
        thr   = new Thread(this,"estanquero");
    }

    public void run(){

        try{

            int ingrediente;
            
	    while (true){
		
                ingrediente = (int) (Math.random() * 3.0); // 0,1 o 2
		
		//if(estanco.recuperarValor() == ingrediente){
			//System.out.println("Se ha puesto el mismo ingrediente que antes: " + ingrediente);
		//}

                estanco.ponerIngrediente(ingrediente);
                estanco.esperarRecogidaIngrediente();
            }
        }

        catch(Exception e){

            System.err.println("Excepcion en main: " + e);
        }
    }
}


// ****************************************************************************


class Fumador implements Runnable{

    private Estanco estanco;
    private int miIngrediente;;
    public Thread thr;

    public Fumador(Estanco pMonEstanco, int p_miIngrediente){

        estanco    = pMonEstanco;
        miIngrediente  = p_miIngrediente;
        thr   = new Thread(this,"fumador " + p_miIngrediente);
    }

    public void run(){

        try{

            while(true){

                estanco.obtenerIngrediente(miIngrediente);
                auxFumadores.dormir_max(2000);
            }
        }

        catch(Exception e){

            System.err.println("Excepcion en main: " + e);
        }
    }
}


class auxFumadores{

    static Random genAlea = new Random();

    static void dormir_max(int milisecsMax){

        try{

            Thread.sleep(genAlea.nextInt(milisecsMax));
        }

        catch(InterruptedException e){

            System.err.println("sleep interumpido en 'aux.dormir_max()'");
        }
    }
}


// ****************************************************************************


class claseMain{

    public static void main(String[] args){

	int TAM=3;
        Estanco estanco = new Estanco();
        Estanquero estanquero = new Estanquero(estanco);
        Fumador[] fumadores = new Fumador[3]; // Array de 3 fumadores


	// Creamos las hebras de los fumadores
	for(int i=0;i<TAM;i++) fumadores[i]= new Fumador(estanco, i);
      


    	// Ponemos en marcha todas las hebras
        estanquero.thr.start();
	for(int i=0;i<TAM;i++) fumadores[i].thr.start();
        
        


    }
}



