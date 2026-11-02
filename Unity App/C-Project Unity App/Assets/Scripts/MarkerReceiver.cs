using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using Unity.AppUI.UI;
using UnityEngine;

public class MarkerReceiver : MonoBehaviour
{
    public int port = 9000;

    public RotationScript rotationScript;

    [Tooltip("Reset values if no message has arrived for a period of time.")]
    public float resetTimeout = 1.0f;

    [Header("Debug Values")]
    public bool hasData;
    public int markerId = -1;
    public float zoom;
    public float pitch;
    public float yaw;
    public float roll;
    public float lastReceivedTime;

    private UdpClient udpClient;

    void OnEnable()
    {
        udpClient = new UdpClient(port);
    }

    void OnDisable()
    {
        udpClient?.Close();
        udpClient = null;
    }

    private void Update()
    {
        if (udpClient == null) return;

        IPEndPoint sender = null;
        while (udpClient.Available > 0)
        {
            byte[] data = udpClient.Receive(ref sender);
            TryDecode(data);
            UpdateValues(rotationScript);
        }

        if (hasData && Time.time - lastReceivedTime > resetTimeout)
        {
            hasData = false;
            markerId = -1;
            zoom     = 0f;
            pitch    = 0f;
            yaw      = 0f;
            roll     = 0f;
            UpdateValues(rotationScript);
        }
    }

    private void UpdateValues(RotationScript script)
    {
        script.id = markerId;
        script.zoom = zoom;
        script.pitch = pitch;
        script.yaw = yaw;
        script.roll = roll;
    }

    private bool TryDecode(byte[] data)
    {
        int pos = 0;

        if (ReadPaddedString(data, ref pos) != "/marker") return false;
        if (ReadPaddedString(data, ref pos) != ",iffff") return false;
        if (data.Length < pos + 20) return false; // 1 int + 4 floats = 20 bytes, checks if the amount of data is correct

        markerId = ReadInt32(data, ref pos);
        zoom     = ReadFloat(data, ref pos);
        pitch    = ReadFloat(data, ref pos);
        yaw      = ReadFloat(data, ref pos);
        roll     = ReadFloat(data, ref pos);
        hasData = true;
        lastReceivedTime = Time.time;
        return true;
    }

    private static string ReadPaddedString(byte[] data, ref int pos)
    {
        int start = pos;
        while (pos < data.Length && data[pos] != 0) pos++;
        string text = Encoding.ASCII.GetString(data, start, pos - start);

        pos++;
        while (pos % 4 != 0) pos++; // Skip padding to align to 4 bytes
        return text;
    }

    private static int ReadInt32(byte[] data, ref int pos)
    {
        int value = (data[pos] << 24) | (data[pos + 1] << 16) | (data[pos + 2] << 8) | data[pos + 3];
        pos += 4;
        return value;
    }

    private static float ReadFloat(byte[] data, ref int pos)
    {
        int bits = ReadInt32(data, ref pos);
        return BitConverter.Int32BitsToSingle(bits);
    }
}