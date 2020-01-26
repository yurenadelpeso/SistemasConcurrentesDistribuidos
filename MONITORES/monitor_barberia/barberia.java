import java.util.Random;
import monitor.*;
class Barberia extends AbstractMonitor{
    private Condition sala_espera   = makeCondition();
    private Condition barbero       = makeCondition();
    private Condition silla         = makeCondition();
    // Invocado por los clientes para cortarse el pelo
    public void cortarPelo(){
        enter();
            // Si la silla está ocupada...
            if (!silla.isEmpty()){
                // .. pasamos al cliente a la sala de espera
                System.out.println("Silla ocupada ");
                sala_espera.await();

            }
            // Pelamos al cliente (despertamos al barbero y ponemos al cliente en la silla)
            System.out.println("Cliente empieza a afeitarse ");
            barbero.signal();
            silla.await();
        leave();
    }
    // Invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
    public void siguienteCliente(){
        enter();
            // Si la sala y la silla están vacias...
            if (sala_espera.isEmpty() && silla.isEmpty())
            {
                // ...ponemos al barbero a dormir
                System.out.println("Barbero se pone a dormir ");
                barbero.await();
            }
            // Sacamos al siguiente cliente de la sala de espera
            System.out.println("Barbero coge otro cliente ");
            sala_espera.signal();
        leave();
    }
    // Invocado por el barbero para indicar que ha terminado de cortar el pelo
    public void finCliente(){
        enter();
            // Sacamos al cliente de la silla (estaba en espera la hebra mientras se pelaba)
            System.out.println("Barbero termina de afeitar ");
            silla.signal();
        leave();
    }
}
class Cliente implements Runnable{
    private Barberia barberia;
    public Thread thr;
    public Cliente(Barberia mon){
        barberia = mon;
        thr   = new Thread(this,"cliente ");
    }
    public void run(){
        while (true){
            try{
                barberia.cortarPelo();  // el cliente espera (si procede) y se corta el pelo
                auxBarbero.dormir_max( 2000 ); // el cliente esta fuera de la barberia un tiempo
            }
            catch(Exception e){
                System.err.println("Excepcion en main: " + e);
            }
        }
    }
}
class Barbero implements Runnable{
    private Barberia barberia;
    public Thread thr;
    public Barbero(Barberia mon){
        barberia = mon;
        thr   = new Thread(this,"barbero");
    }
    public void run(){
        while (true){
            try{
                barberia.siguienteCliente();
                auxBarbero.dormir_max( 2500 ); // el barbero esta cortando el pelo
                barberia.finCliente();
            }
            catch(Exception e){
                System.err.println("Excepcion en main: " + e);
            }
        }
    }
}
class auxBarbero{
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
class MainBarbero{
    public static void main(String[] args){
        final int NUM_CLIENTES = 5;
        /* 
            Creamos los objetos de la clase-monitor "Barberia", de la clase "Barbero" y el buffer intermedio 
            para la clase "Cliente" 
        */
        Barberia barberia = new Barberia();
        Barbero barbero = new Barbero(barberia);
        Cliente[] clientes = new Cliente[NUM_CLIENTES];
        // Creamos cada cliente del buffer intermedio
        for (int i=0; i<NUM_CLIENTES; i++)
            clientes[i] = new Cliente(barberia);
        // Ponemos en marcha todas las hebras      
        barbero.thr.start();
        for (int i=0; i<NUM_CLIENTES; i++)
            clientes[i].thr.start();
        auxBarbero.dormir_max( 8500 ); // El barbero esta cortando el pelo
    }

}
