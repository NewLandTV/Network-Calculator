using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using TMPro;
using UnityEngine;

public enum Operators
{
    Add,
    Subtract,
    Multiply,
    Divide
}

public class Calculator : MonoBehaviour
{
    [SerializeField]
    private TextMeshProUGUI calculateBufferText;

    private StringBuilder calculateBuffer = new StringBuilder();

    private Socket socket;

    private void Awake()
    {
        socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);

        socket.Connect(IPAddress.Parse("127.0.0.1"), 2694);
    }

    public void OnNumberButtonClick(int number)
    {
        calculateBuffer.Append(number);

        calculateBufferText.text = calculateBuffer.ToString();
    }

    public void OnOperatorButtonClick(int operators)
    {
        if (calculateBuffer.Length <= 0)
        {
            calculateBuffer.Append(0);
        }

        if (calculateBuffer[calculateBuffer.Length - 1] == ' ')
        {
            return;
        }

        switch ((Operators)operators)
        {
            case Operators.Add:
                calculateBuffer.Append(" + ");

                break;
            case Operators.Subtract:
                calculateBuffer.Append(" - ");

                break;
            case Operators.Multiply:
                calculateBuffer.Append(" x ");

                break;
            case Operators.Divide:
                calculateBuffer.Append(" ¡À ");

                break;
        }

        calculateBufferText.text = calculateBuffer.ToString();
    }

    public void OnCalculateButtonClick()
    {
        calculateBuffer.Append(" =");

        byte[] data = Encoding.Unicode.GetBytes(calculateBuffer.ToString());

        socket.Send(data, data.Length, SocketFlags.None);

        byte[] buffer = new byte[2048];

        socket.Receive(buffer, 2048, SocketFlags.None);

        calculateBufferText.text = Encoding.ASCII.GetString(buffer);

        calculateBuffer.Clear();

        if (calculateBuffer.ToString().Equals("exit", StringComparison.OrdinalIgnoreCase))
        {
            socket.Close();

            socket = null;
        }
    }

    public void OnClearButtonClick()
    {
        calculateBuffer.Clear();

        calculateBufferText.text = "0";
    }
}
