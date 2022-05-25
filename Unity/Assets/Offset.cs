using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "Offset", menuName = "ScriptableObjects/Offset", order = 2)]
public class Offset : ScriptableObject
{

    public float offset;

    public float GetWithOffset(float value) => value + offset;

}
