using UnityEngine;

public class DataDisplayUI : MonoBehaviour
{
    public MarkerReceiver markerReceiver;
    public Rect area = new Rect(10, 10, 240, 170);
    public int fontSize = 16;

    private GUIStyle style;

    void Awake()
    {
        if (markerReceiver == null) markerReceiver = GetComponent<MarkerReceiver>();
    }

    void OnGUI()
    {
        if (markerReceiver == null) return;

        if (style == null)
        {
            style = new GUIStyle(GUI.skin.label) { fontSize = fontSize, richText = true };
            style.normal.textColor = Color.white;
        }

        string text;
        if (markerReceiver.hasData)
        {
            text = "<color=lime>Marker detected</color>\n" +
                   "ID: " + markerReceiver.markerId + "\n" +
                   "Zoom: " + markerReceiver.zoom.ToString("F2") + "\n" +
                   "Pitch: " + markerReceiver.pitch.ToString("F2") + "°\n" +
                   "Yaw: " + markerReceiver.yaw.ToString("F2") + "°\n" +
                   "Roll: " + markerReceiver.roll.ToString("F2") + "°";
        }
        else
        {
            text = "<color=red>No marker detected</color>";
        }

        GUI.Box(area, GUIContent.none);
        GUI.Label(new Rect(area.x + 10, area.y + 8, area.width - 20, area.height - 16), text, style);
    }
}