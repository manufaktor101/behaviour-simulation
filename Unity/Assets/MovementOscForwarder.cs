using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class MovementOscForwarder : MonoBehaviour
{
    [SerializeField] private OSC osc;
    [SerializeField] private string oscPath;

    // Start is called before the first frame update
    void Start()
    {
    }
    

    // Update is called once per frame
    void Update()

    {
        if (!Input.anyKeyDown)
            return;
        
        RobotCommand command = RobotCommand.STOP;

        if (Input.GetKeyDown(KeyCode.LeftArrow))
            command = RobotCommand.LEFT;
        else if (Input.GetKeyDown(KeyCode.RightArrow))
            command = RobotCommand.RIGHT;
        else if (Input.GetKeyDown(KeyCode.UpArrow))
            command = RobotCommand.FORWARD;
        else if (Input.GetKeyDown(KeyCode.DownArrow))
            command = RobotCommand.STOP;

        // TODO Back not implemented yet

        SendOscCommand(command);
    }

    private void SendOscCommand(RobotCommand command)
    {
        var oscMessage = new OscMessage();
        oscMessage.address = oscPath;

        oscMessage.values.Add((int)command);
        osc.Send(oscMessage);
    }
}