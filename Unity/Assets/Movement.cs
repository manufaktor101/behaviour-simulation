using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour
{
    public int movementCoefficient = 10;

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        var (axisHorizontal,axisVertical,isQPressed) = GetKeyboardInputs();

        MoveAndRotate(axisHorizontal, axisVertical, isQPressed);
    }

    private void MoveAndRotate(float axisHorizontal, float axisVertical, bool isQPressed)
    {
        transform.Translate(
            axisHorizontal * movementCoefficient * Time.deltaTime,
            0,
            axisVertical * movementCoefficient * Time.deltaTime);

        if (isQPressed)
            transform.RotateAround(transform.position, Vector3.up, 10);
    }

    private static (float axisVertical, float axisHorizontal, bool isQPressed) GetKeyboardInputs()
    {
        var axisHorizontal = Input.GetAxis("Horizontal");
        var axisVertical = Input.GetAxis("Vertical");
        var isQPressed = Input.GetKey(KeyCode.Q);
        return (axisHorizontal, axisVertical, isQPressed);
    }
}