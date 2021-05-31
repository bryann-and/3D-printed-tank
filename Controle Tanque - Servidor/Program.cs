using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;

namespace ControleTanqueServidor
{
    public class Dados
    {
        public int MotorEsquerdo { get; set; }
        public int MotorDireito { get; set; }
    }
    class Program
    {
        private static Socket Listener { get; set; }
        private static Socket Socket { get; set; }
        private static XInputController Controle { get; set; }

        static void Main(string[] args)
        {
            try
            {
                Dados dados = new Dados();
                Controle = new XInputController();
                IniciarConexao();

                while (true)
                {
                    Controle.Update();

                    //Console.WriteLine(Controle.leftThumb.Y + "|" + Controle.rightThumb.Y);

                    dados.MotorEsquerdo = Controle.leftThumb.Y;
                    dados.MotorDireito = Controle.rightThumb.Y;

                    EnviarInformacoes(JsonSerializer.Serialize(dados));

                    System.Threading.Thread.Sleep(10);
                }
            }
            catch (Exception e)
            {
                throw;
            }
            finally
            {
                Socket.Shutdown(SocketShutdown.Both);
                Socket.Close();
            }
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
            Console.WriteLine("Conectado");
        }

        static void EnviarInformacoes(string msg)
        {
            // Enviando msg
            Socket.Send(Encoding.ASCII.GetBytes(msg + ";"));
        }
    }


}
