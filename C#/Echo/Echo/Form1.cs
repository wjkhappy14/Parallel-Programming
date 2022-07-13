namespace Echo
{
    public partial class Form1 : Form
    {
        ReceiveMsg receiveMsg= new ReceiveMsg();

        public Form1()
        {
            InitializeComponent();
        }

        private async void btnOK_Click(object sender, EventArgs e)
        {
            int t = await receiveMsg.WaitForDataAsync();
        }
    }
}