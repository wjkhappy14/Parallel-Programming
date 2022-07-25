using System;
using System.Buffers;
using System.IO;
using System.IO.Pipelines;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace TcpEcho
{
    class Program
    {
        static async Task Main(string[] args)
        {
            Socket socket = new Socket(SocketType.Stream, ProtocolType.Tcp);

            Console.WriteLine("Connecting to port 8087");

           // socket.Connect(new IPEndPoint(IPAddress.Loopback, 8087));
           /// NetworkStream stream = new NetworkStream(socket);

            Stream ss = Console.OpenStandardInput();
           // await ss.CopyToAsync(stream);

            PipeReader pipeReader = PipeReader.Create(ss);
            PipeWriter pipeWriter = PipeWriter.Create(ss);

            MemoryPool<byte> pool = MemoryPool<byte>.Shared;

            //System.Buffers.ArrayMemoryPool<byte> arrayMemoryPool = new System.Buffers.ArrayMemoryPool<byte>();

        PipeOptions pipeOptions = new PipeOptions(pool, PipeScheduler.Inline, PipeScheduler.Inline, 1, 1, 1, false);

            Pipe pipe = new Pipe(pipeOptions);

        }
    }
}
