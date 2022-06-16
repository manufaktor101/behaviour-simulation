using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// maps pozyx values and sets transform and rotation of object
public class PozyxTransformer : MonoBehaviour
{
    public float pozyxX;
    public float pozyxY;

    public float heading;
    
    public CoordinateMapper coordinateMapperX;
    public CoordinateMapper coordinateMapperY;

    public Offset headingOffset;
    void Update()
    {
        var pos = new Vector3()
        {
            x = coordinateMapperX.Map(pozyxX),
            y = transform.position.y,
            z = coordinateMapperY.Map(pozyxY)
        };

        var headingWithOffset = headingOffset.GetWithOffset(heading);
        transform.SetPositionAndRotation(pos,Quaternion.Euler(0,headingWithOffset,0));
    }
}
