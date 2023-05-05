using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.Serialization.Formatters.Binary;


public enum LayerState
{    
    L2Part0 = 6,
    L2Part1,
    L2Part2,
    LJinggum,
    L2Part4,
    L2Part5,
    L2Part6,
    L2Part7,
    L2Part8,
    L1Water,
    L1Part1,
    L1Part2,
    L1Part3,
    L1Part4
}

[System.Serializable] public struct SerializedVector3
{
    public float x;
    public float y;
    public float z;

    public SerializedVector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public SerializedVector3(Vector3 v)
    {
        this.x = v.x;
        this.y = v.y;
        this.z = v.z;
    }

    public override bool Equals(object obj)
    {
        if ((obj is SerializedVector3) == false)
        {
            return false;
        }

        var s = (SerializedVector3)obj;
        return x == s.x && y == s.y && z == s.z;
    }

    public override int GetHashCode()
    {
        var hashCode = 373119288;
        hashCode = hashCode * -1521134295 + x.GetHashCode();
        hashCode = hashCode * -1521134295 + y.GetHashCode();
        hashCode = hashCode * -1521134295 + z.GetHashCode();
        return hashCode;
    }

    public Vector3 ToVector3()
    {
        return new Vector3(x, y, z);
    }

    public static bool operator ==(SerializedVector3 a, SerializedVector3 b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    public static bool operator !=(SerializedVector3 a, SerializedVector3 b)
    {
        return a.x != b.x || a.y != b.y || a.z != b.z;
    }

    public static implicit operator Vector3(SerializedVector3 x)
    {
        return new Vector3(x.x, x.y, x.z);
    }

    public static implicit operator SerializedVector3(Vector3 x)
    {
        return new SerializedVector3(x.x, x.y, x.z);
    }

}

[System.Serializable] class MapObject
{

    public SerializedVector3 vPos;
    public SerializedVector3 vScale;
    public LayerState eState;

    public void SetValue(SerializedVector3 _vPos, SerializedVector3 _vScale, LayerState _eState)
    {
        vPos = _vPos;
        vScale = _vScale;
        eState = _eState;
    }
};

public class ExportBin : MonoBehaviour
{
    public string pathName = "MapPosition.bin";
    float fValue = 400f; //����Ƽ->�����ӿ�ũ ���� ������
    int FLOORHEIGET = 400;

    //�ʿ�����Ʈ���� ����
    private List<MapObject> mapObjects = new List<MapObject>(); // MapObject ��ü�� ������ List
    private void SaveMapObjects()
    {
        BinaryFormatter bf = new BinaryFormatter();
        FileStream file = File.Create(pathName);
        bf.Serialize(file, mapObjects);
        file.Close();
    }

    // ���̳ʸ� �������� ����� Vector Container�� �о���� �Լ�
    private void LoadMapObjects()
    {
        if (File.Exists(pathName))
        {
            BinaryFormatter bf = new BinaryFormatter();
            FileStream file = File.Open(pathName, FileMode.Open);
            mapObjects = (List<MapObject>)bf.Deserialize(file);
            file.Close();
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        ////���� �ִ� ��� ������Ʈ���� ��������
        ////2�� : y 10 ���� || 1�� : 10 - FLOORHEIGET(=400)
        //GameObject[] objects = FindObjectsOfType<GameObject>();
        //foreach (GameObject obj in objects)
        //{
        //    MapObject mpobj = new MapObject(); // ��ü �ν��Ͻ�ȭ
        //    mpobj.SetValue(obj.transform.position * fValue , obj.transform.localScale, (LayerState)obj.layer);
        //    mpobj.vPos.y = 10.0f - FLOORHEIGET; 
        //    mapObjects.Add(mpobj);

        //}
        //SaveMapObjects();
        ////////////////////////////////////////////////////////////////////////////
        LoadMapObjects();
        //�о���� �׽�Ʈ

        print(mapObjects[0].vPos.z);
        print(mapObjects[0].vPos.y);
        print(mapObjects[0].eState);
        print("================");
        print(mapObjects[600].vPos.z);
        print(mapObjects[600].vPos.y);
        print(mapObjects[600].eState);

    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
