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

        private static int SteeringReducer = 2;

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
                    dados.MotorEsquerdo = dados.MotorDireito = Controle.leftThumb.Y;

                    // Neutral steering
                    if (Controle.leftThumb.Y == 0)
                    {
                        if (Controle.leftThumb.X > 0)
                        {
                            dados.MotorEsquerdo = Controle.leftThumb.X;
                            dados.MotorDireito = -Controle.leftThumb.X;
                        }
                        else if (Controle.leftThumb.X < 0)
                        {
                            dados.MotorEsquerdo = Controle.leftThumb.X;
                            dados.MotorDireito = -Controle.leftThumb.X;
                        }
                    }
                    // Moving forward
                    else if (Controle.leftThumb.Y > 0)
                    {
                        // Turning right
                        if (Controle.leftThumb.X > 0)
                        {
                            dados.MotorEsquerdo = Controle.leftThumb.Y;
                            dados.MotorDireito = Controle.leftThumb.Y - Controle.leftThumb.X / SteeringReducer;
                        }
                        // Turning left
                        else if (Controle.leftThumb.X < 0)
                        {
                            dados.MotorEsquerdo = Controle.leftThumb.Y - Math.Abs(Controle.leftThumb.X) / SteeringReducer;
                            dados.MotorDireito = Controle.leftThumb.Y;
                        } 
                    }
                    // Moving backwards
                    else
                    {
                        // Turning right
                        if (Controle.leftThumb.X > 0)
                        {
                            dados.MotorEsquerdo = Controle.leftThumb.Y;
                            dados.MotorDireito = Controle.leftThumb.Y + Controle.leftThumb.X / SteeringReducer;
                        }
                        // Turning left
                        else if (Controle.leftThumb.X < 0)
                        {
                            dados.MotorEsquerdo = Controle.leftThumb.Y - Controle.leftThumb.X / SteeringReducer;
                            dados.MotorDireito = Controle.leftThumb.Y;
                        }
                    }

                    Console.WriteLine(dados.MotorEsquerdo + "|" + dados.MotorDireito);

                    EnviarInformacoes(JsonSerializer.Serialize(dados));

                    System.Threading.Thread.Sleep(100);
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
