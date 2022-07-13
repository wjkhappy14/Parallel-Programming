using System.Net;
using System.Net.Sockets;

namespace Echo
{
    public partial class Form1 : Form
    {
        private TcpClient TcpClient { get; }
        ReceiveMsg receiveMsg = new ReceiveMsg();

        public Form1()
        {
            InitializeComponent();
            TcpClient = new TcpClient();
        }

        private async void btnOK_Click(object sender, EventArgs e)
        {
            byte[]? continueBytes = new byte[] { (byte)'H', (byte)'T', (byte)'T', (byte)'P', (byte)'/', (byte)'1', (byte)'.', (byte)'1', (byte)' ', (byte)'1', (byte)'0', (byte)'0', (byte)' ', (byte)'C', (byte)'o', (byte)'n', (byte)'t', (byte)'i', (byte)'n', (byte)'u', (byte)'e', (byte)'\r', (byte)'\n', (byte)'\r', (byte)'\n' };

            IPEndPoint? endPoint = new IPEndPoint(IPAddress.Parse("106.13.130.51"), 8888);//°Ù¶È
            try
            {
                TcpClient.Connect(endPoint);
                TcpClient.Client.Send(continueBytes);
            }
            catch (Exception e)
            {

            }
            int t = await receiveMsg.WaitForDataAsync();
        }
    }
}