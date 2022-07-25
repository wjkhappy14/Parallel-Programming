using System;
using System.Buffers;
using System.Collections.Concurrent;
using System.IO.Pipelines;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;


namespace Server
{
    class Program
    {
        static async Task Main(string[] args)
        {
            ConcurrentDictionary<EndPoint, Socket> clients = new ConcurrentDictionary<EndPoint, Socket>();

            IPEndPoint endpoint = new IPEndPoint(IPAddress.Any, 8087);

            Socket socket = new Socket(SocketType.Stream, ProtocolType.Tcp);
            socket.Bind(endpoint);

            Console.WriteLine($"Socket监听: {endpoint}");

            socket.Listen(120);

            while (true)
            {
                Socket client = await socket.AcceptAsync();
                clients.TryAdd(client.RemoteEndPoint, client);
                _ = ProcessLinesAsync(client);
            }
        }

        private static async Task ProcessLinesAsync(Socket socket)
        {
            //  Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine($"[{socket.RemoteEndPoint}]: 连接 ");

            // Create a PipeReader over the network stream
            NetworkStream stream = new NetworkStream(socket);
            PipeReader reader = PipeReader.Create(stream);
            ReadResult result = default(ReadResult);
            do
            {
                result = await reader.ReadAsync();
                ReadOnlySequence<byte> buffer = result.Buffer;

                while (TryReadLine(ref buffer, out ReadOnlySequence<byte> line))
                {
                    // Process the line.
                    ProcessLine(line);
                }

                // Tell the PipeReader how much of the buffer has been consumed.
                reader.AdvanceTo(buffer.Start, buffer.End);

                // Stop reading if there's no more data coming.

            }
            while (result.IsCompleted);

            // Mark the PipeReader as complete.
            await reader.CompleteAsync();

            Console.WriteLine($"[{socket.RemoteEndPoint}]: disconnected");
        }

        private static bool TryReadLine(ref ReadOnlySequence<byte> buffer, out ReadOnlySequence<byte> line)
        {
            // Look for a EOL in the buffer.
            SequencePosition? position = buffer.PositionOf((byte)'\n');

            if (position == null)
            {
                line = default;
                return false;
            }

            // Skip the line + the \n.
            line = buffer.Slice(0, position.Value);
            buffer = buffer.Slice(buffer.GetPosition(1, position.Value));
            return true;
        }

        private static void ProcessLine(in ReadOnlySequence<byte> buffer)
        {
            foreach (ReadOnlyMemory<byte> segment in buffer)
            {
                byte[] s = segment.ToArray();

                Console.Write(Encoding.UTF8.GetString(s));
                Console.WriteLine();
            }
        }
    }
}
