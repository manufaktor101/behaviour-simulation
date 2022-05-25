using System;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(PozyxTransformer))]
public class OscListener : MonoBehaviour
{
    public OSC osc;

    public string oscPath;

    private PozyxTransformer _pozyxTransformer;
    
    // Start is called before the first frame update
    void Start()
    {
        _pozyxTransformer = GetComponent<PozyxTransformer>();
        osc.SetAddressHandler( oscPath , OnPosition );
    }

    private void OnPosition(OscMessage message)
    {
        // Debug.Log($"Received OSC packet with {oscPath}");
        var x = message.GetInt(0);
        var y = message.GetInt(1);
        var z = message.GetInt(2);
        var heading = message.GetFloat(3);

        SetPozyxTransformerConfiguration(x, y, z, heading);
    }

    private void SetPozyxTransformerConfiguration(int x, int y, int z, float heading)
    {
        _pozyxTransformer.pozyxX = x;
        _pozyxTransformer.pozyxY = y;
        _pozyxTransformer.heading = heading;
    }
}
