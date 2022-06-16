using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class FollowCharacter : MonoBehaviour
{
    public GameObject destination;


    private NavMeshAgent agent;
    // Start is called before the first frame update
    void Start()
    {
         agent = GetComponent<NavMeshAgent>();
    }

    // Update is called once per frame
    void Update()
    {
        agent.destination = destination.transform.position;
    }
}