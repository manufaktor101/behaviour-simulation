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


    private Vector3? destination = null;


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

            if (calculatedAngleDebug < -50)
                SendOscCommand(RobotCommand.LEFT);
            else if (calculatedAngleDebug > 50)
                SendOscCommand(RobotCommand.RIGHT);
            else
            {
                // are we close?
                var magnitude = vecRobotDestination.magnitude;
                // var mappedDistance = distanceMapper.Map(20);
                calculatedDistanceDebug = magnitude;
                if (magnitude < 3)
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

    private void SendOscCommand(RobotCommand command)
    {
        var oscMessage = new OscMessage();
        oscMessage.address = oscPath;

        oscMessage.values.Add((int)command);
        osc.Send(oscMessage);
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