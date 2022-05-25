using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DebugFormatter : MonoBehaviour
{
    // Start is called before the first frame update
    private Canvas canvas;

    void Start()
    {
        canvas = GetComponent<Canvas>();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.D))
            // gameObject.SetActive(!gameObject.activeSelf);
            canvas.enabled = !canvas.enabled;
    }
}