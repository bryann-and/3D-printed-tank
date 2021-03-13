using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace ControleTanqueServidor
{
    class Program
    {
        static void Main(string[] args)
        {
            IniciarServidor();
        }

        static void IniciarServidor()
        {
            Console.WriteLine("Iniciando o servidor");

            IPAddress enderecoIP = IPAddress.Any;
            IPEndPoint localEndPoint = new IPEndPoint(enderecoIP, 11000);

            try
            {
                Socket listener = new Socket(enderecoIP.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                listener.Bind(localEndPoint);
                listener.Listen(10);

                Console.WriteLine("Aguardando conexão.");
                Socket handler = listener.Accept();

                string data = null;
                byte[] bytes = null;

                while (true)
                {
                    bytes = new byte[1024];
                    int bytesRecebidos = handler.Receive(bytes);
                    data += Encoding.ASCII.GetString(bytes, 0, bytesRecebidos);
                    if (data.IndexOf("<EOF>") > -1)
                    {
                        break;
                    }

                    Console.WriteLine("Texto recebido:" + data);
                    handler.Shutdown(SocketShutdown.Both);
                    handler.Close();
                }
            }
            catch (Exception)
            {
                throw;
            }
            finally
            {
                Console.WriteLine("\n Press any key to continue...");
                Console.ReadKey();
            }
        }
    }
}
