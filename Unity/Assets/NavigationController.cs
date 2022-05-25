using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class NavigationController : MonoBehaviour
{
    [SerializeField] private CoordinateMapper coordinateMapperX;
    [SerializeField] private CoordinateMapper coordinateMapperY;

    [SerializeField] private CoordinateMapper distanceMapper;

    [SerializeField] private GameObject destinationMarker;

    [SerializeField] private OSC osc;

    [SerializeField] private string oscPath;


    [SerializeField] private float calculatedAngleDebug;
    [SerializeField] private float calculatedDistanceDebug;
    [SerializeField] private float SetMinMagnitude;
    [SerializeField] private float SetMinAngle;
    


    private Vector3? destination = null;


    private void Start()
    {
        Debug.Log("Starting navigation controller");
    }

    // Update is called once per frame
    void Update()
    {
        SetNewDestinationIfRequested();

        if (destination != null)
        {
            // calculate vector in world space from heading with offset(deg)
            var vecHeading = transform.rotation * Vector3.forward;

            // calculate angle between robot heading and vector between robot and destination
            var vecRobotDestination = (Vector3)destination - transform.position;
            calculatedAngleDebug =
                Vector3.SignedAngle(vecHeading, vecRobotDestination, Vector3.up);

            // decide, whether that means
            // - correct, no turn needed
            // - turn left
            // - turn right

            if (calculatedAngleDebug < (-1* SetMinAngle))
                SendOscCommand(RobotCommand.LEFT);
            else if (calculatedAngleDebug > SetMinAngle)
                SendOscCommand(RobotCommand.RIGHT);
            else
            {
                // are we close?
                var magnitude = vecRobotDestination.magnitude;
                // var mappedDistance = distanceMapper.Map(20);
                calculatedDistanceDebug = magnitude;
                if (magnitude < SetMinMagnitude)
                    SendOscCommand(RobotCommand.STOP);
                else
                {
                   // SendOscCommand(RobotCommand.STOP);
                    SendOscCommand(RobotCommand.FORWARD);
                }
            }
        }
    }


    // Update is called once per frame
    private RobotCommand commandLast = RobotCommand.STOP;
    private void SendOscCommand(RobotCommand command)
    {
        if(command != commandLast)
        {
            Debug.Log($"Sent { command} to { oscPath} (lastCOmmand: {commandLast})");
            var oscMessage = new OscMessage();
            oscMessage.address = oscPath;

            oscMessage.values.Add((int)command);
            osc.Send(oscMessage);

            commandLast = command;

        }

  

        //Debug.Log($"Sent {command} to {oscPath}");
    }

    private bool SetNewDestinationIfRequested()
    {
        if (Input.GetMouseButtonDown(0))
        {
            RaycastHit hit;

            if (Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hit, 100))
            {
                destination = hit.point;
                destinationMarker.transform.position = hit.point;
                return true;
            }
        }

        return false;
    }
}