using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "CoordinateMapper_Axis", menuName = "ScriptableObjects/CoordinateMapper", order = 1)]
public class CoordinateMapper : ScriptableObject
{
    public float inputLowerBound;
    public float inputUpperBound;
    public float outputLowerBound;
    public float outputUpperBound;

    public float Map(float value)
    {
        var normal = Mathf.InverseLerp(inputLowerBound, inputUpperBound, value); // map posyx reading to value between 0 and 1
        return Mathf.Lerp(outputLowerBound, outputUpperBound, normal); // map unity coordinate with normalized posyx reading
    }
}
