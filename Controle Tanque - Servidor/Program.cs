using System;
using System.Net;
using System.Net.Sockets;
using System.Text;


namespace ControleTanqueServidor
{
    class Program
    {
        private static Socket Listener { get; set; }
        private static Socket Socket { get; set; }
        private static XInputController Controle { get; set; }

        static void Main(string[] args)
        {
            Controle = new XInputController();
            IniciarConexao();

            while (true)
            {
                Controle.Update();
                Console.WriteLine(Controle.leftThumb + " ---" + Controle.rightThumb);
                EnviarInformacoes(Controle.leftThumb.Y.ToString());
                System.Threading.Thread.Sleep(10);
            }
            
            Console.WriteLine("\n Press any key to continue...");
            Console.ReadKey();
            Socket.Shutdown(SocketShutdown.Both);
            Socket.Close();
        }

        static void IniciarConexao()
        {
            Console.WriteLine("Iniciando o servidor");

            IPAddress enderecoIP = IPAddress.Any;
            IPEndPoint localEndPoint = new IPEndPoint(enderecoIP, 11000);

            Listener = new Socket(enderecoIP.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            Listener.Bind(localEndPoint);
            Listener.Listen(10);

            Console.WriteLine("Aguardando conexão.");
            Socket = Listener.Accept();
        }

        static void EnviarInformacoes(string msg)
        {
            // Enviando msg
            Socket.Send(Encoding.ASCII.GetBytes(msg + ";"));
        }
    }


}
