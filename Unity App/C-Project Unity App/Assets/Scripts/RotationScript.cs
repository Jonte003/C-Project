using Unity.VisualScripting;
using UnityEngine;

public class RotationScript : MonoBehaviour
{
    public GameObject prefabObject1;
    public GameObject prefabObject2;
    public GameObject prefabObject3;

    public GameObject spawnedObject = null;

    public float pitch;
    public float yaw;
    public float roll;
    public float zoom;
    public int id;

    public int spawnedId = -1;

    void Start()
    {

    }

    void Update()
    {
        if (id == -1) //Destoy object if marker lost
        {
            if (spawnedObject != null)
            {
                Destroy(spawnedObject);
                spawnedObject = null;
            }
            spawnedId = -1;
            return;
        }

        if (id != spawnedId) //If id changes
        {
            if (spawnedObject != null) //Destoy old objecct
            {
                Destroy(spawnedObject);
                spawnedObject = null;
            }

            if (id >= 0 && id <= 2) //Spawn new object
            {
                SpawnByID(id);
            }
            else
            {
                spawnedId = id;
            }
        }
        else //If ID has not changed
        {
            if (spawnedObject != null && id >= 0 && id <= 2)
            {
                UpdateRotation();
            }
        }
    }

    private void SpawnByID(int id)
    {
        GameObject prefabToSpawn = null;
        switch (id)
        {
            case 0:
                prefabToSpawn = prefabObject1;
                break;
            case 1:
                prefabToSpawn = prefabObject2;
                break;
            case 2:
                prefabToSpawn = prefabObject3;
                break;
        }
        if (prefabToSpawn != null)
        {
            Quaternion spawnRotation = Quaternion.Euler(pitch, yaw, roll);
            spawnedObject = Instantiate(prefabToSpawn, transform.position, spawnRotation);
            spawnedObject.transform.localScale = new Vector3(zoom, zoom, zoom);
            spawnedId = id;
        }
    }

    private void UpdateRotation()
    {
        Quaternion newRotation = Quaternion.Euler(pitch, yaw, roll);

        spawnedObject.transform.localScale = new Vector3(zoom, zoom, zoom);
        spawnedObject.transform.rotation = newRotation;
    }
}