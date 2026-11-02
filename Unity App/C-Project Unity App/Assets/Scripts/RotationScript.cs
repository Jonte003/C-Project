using Unity.VisualScripting;
using UnityEngine;

public class RotationHandler : MonoBehaviour
{
    public GameObject prefabObject1;
    public GameObject prefabObject2;
    public GameObject prefabObject3;

    public GameObject spawnedObject = null;

    public int rotation;
    public int size;
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

            if (id >= 1 && id <= 3) //Spawn new object
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
            if (spawnedObject != null && id >= 1 && id <= 3)
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
            case 1:
                prefabToSpawn = prefabObject1;
                break;
            case 2:
                prefabToSpawn = prefabObject2;
                break;
            case 3:
                prefabToSpawn = prefabObject3;
                break;
        }
        if (prefabToSpawn != null)
        {
            Quaternion spawnRotation = Quaternion.Euler(0, rotation, 0);
            spawnedObject = Instantiate(prefabToSpawn, transform.position, spawnRotation);
            spawnedObject.transform.localScale = new Vector3(size, size, size);
            spawnedId = id;
        }
    }

    private void UpdateRotation()
    {
        Quaternion newRotation = Quaternion.Euler(0, rotation, 0);

        spawnedObject.transform.localScale = new Vector3(size, size, size);
        spawnedObject.transform.rotation = newRotation;
    }
}