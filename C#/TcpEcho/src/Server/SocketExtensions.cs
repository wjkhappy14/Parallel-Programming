using System;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Server
{
    static class SocketExtensions
    {
        public static Task<int> ReceiveAsync(this Socket socket, Memory<byte> memory, SocketFlags socketFlags)
        {
            ArraySegment<byte> arraySegment = GetArray(memory);
            return SocketTaskExtensions.ReceiveAsync(socket, arraySegment, socketFlags);
        }

        public static string GetString(this Encoding encoding, ReadOnlyMemory<byte> memory)
        {
            var arraySegment = GetArray(memory);
            return encoding.GetString(arraySegment.Array, arraySegment.Offset, arraySegment.Count);
        }

        private static ArraySegment<byte> GetArray(Memory<byte> memory)
        {
            return GetArray((ReadOnlyMemory<byte>)memory);
        }

        private static ArraySegment<byte> GetArray(ReadOnlyMemory<byte> memory)
        {
            if (!MemoryMarshal.TryGetArray(memory, out ArraySegment<byte> result))
            {
                throw new InvalidOperationException("Buffer backed by array was expected");
            }

            return result;
        }
    }
}
