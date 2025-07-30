using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Photon.Pun;
using Photon.Realtime;
using UnityEngine.UI;
using UnityEngine.SceneManagement;


//MonoBehaviourPunCallbacks은 포톤 PUN서비스의 이벤트를 감지할 수 있는 형태의 MonoBehaviour 스크립트
//예시: override void OnConnectedToMaster()처럼 메소드명을 맞춰서 override로 선언하면
//해당 이벤트가 발생했을 때 자동으로 해당 메소드가 실행된다.
public class Enter_Manager_counselor : MonoBehaviourPunCallbacks
{
    private readonly string appVersion = "1"; // 앱 버전

    public Text connectionInfoText; // 네트워크 정보를 표시할 텍스트
    public Button enterButton;       // 상담소 접속 버튼


    // 홈 이동과 동시에 마스터 서버 접속 시도
    // called third
    private void Start()
    {

        // 접속에 필요한 정보(앱 버전) 설정
        PhotonNetwork.GameVersion = appVersion;

        // 설정한 정보를 가지고 마스터 서버 접속 시도
        // 마스터 서버: 포톤 클라우드 서버이자 매치메이킹을 위한 서버

        // 마스터 서버로의 재접속 시도
        if (PhotonNetwork.IsConnected)
        {
            Debug.Log("만약 이미 연결되어있다면 연결종료");
            PhotonNetwork.Disconnect();
        }
        else {
            Debug.Log("아니면 마스터 서버로의 재접속 시도");
            PhotonNetwork.ConnectUsingSettings();
        }
        
        // 상담소 접속 버튼을 잠시 비활성화
        enterButton.interactable = false;

        Debug.Log("접속 시도 중 스타트");
        // 접속을 시도 중임을 텍스트로 표시
        connectionInfoText.text = "마스터 서버에 접속중...";

    }


  /*  // called first
    private void OnEnable()
    {
        SceneManager.sceneLoaded += OnSceneLoaded;
    }


    // called second
    void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        Debug.Log("OnSceneLoaded: " + scene.name);
        Debug.Log(mode);
*//*
        if (PhotonNetwork.IsConnected)
        {
            PhotonNetwork.Disconnect();
        }*/

      /*  // 접속에 필요한 정보(앱 버전) 설정
        PhotonNetwork.GameVersion = appVersion;

        // 설정한 정보를 가지고 마스터 서버 접속 시도
        // 마스터 서버: 포톤 클라우드 서버이자 매치메이킹을 위한 서버
        PhotonNetwork.ConnectUsingSettings();

        // 상담소 접속 버튼을 잠시 비활성화
        enterButton.interactable = false;
        // 접속을 시도 중임을 텍스트로 표시
        connectionInfoText.text = "마스터 서버에 접속중...";*//*
    }

    // called when the game is terminated
    void OnDisable()
    {
        Debug.Log("OnDisable");
        SceneManager.sceneLoaded -= OnSceneLoaded;
        PhotonNetwork.Disconnect();
    }*/


    // 마스터 서버 접속 성공시 자동 실행
    public override void OnConnectedToMaster()
    {
        // 상담소 접속 버튼을 활성화
        enterButton.interactable = true;
        // 접속 정보 표시
        connectionInfoText.text = "온라인 : 마스터 서버와 연결됨";
    }

    // 마스터 서버 접속 실패시 자동 실행
    public override void OnDisconnected(DisconnectCause cause)
    {
        // 상담소 접속 버튼을 비활성화
        //enterButton.interactable = false;
        // 접속 정보 표시
        connectionInfoText.text = "오프라인 : 마스터 서버와 연결되지 않음\n접속 재시도 중...";

        if (PhotonNetwork.IsConnected)
        {
            PhotonNetwork.Disconnect();
        }
        // 마스터 서버로의 재접속 시도
        PhotonNetwork.ConnectUsingSettings();
    }

    // 상담소 접속 시도
    public void Connect()
    {
        // 중복 접속 시도를 막기 위해 접속 버튼 잠시 비활성화
        enterButton.interactable = false;

        // 마스터 서버에 접속중이라면
        if (PhotonNetwork.IsConnected)
        {
            // 상담소 접속 실행
            connectionInfoText.text = "상담소로 이동...";
            // 랜덤 방 참가(임시)
            PhotonNetwork.JoinRandomRoom();
        }
        else
        {
            // 마스터 서버에 접속중이 아니라면, 마스터 서버에 접속 시도
            connectionInfoText.text = "오프라인 : 마스터 서버와 연결되지 않음\n접속 재시도 중...";
            // 마스터 서버로의 재접속 시도
            if (PhotonNetwork.IsConnected)
            {
                PhotonNetwork.Disconnect();
            }
            PhotonNetwork.ConnectUsingSettings();
        }
    }

    // (빈 방이 없어)랜덤 방 참가에 실패한 경우 자동 실행
    public override void OnJoinRandomFailed(short returnCode, string message)
    {
        Debug.Log("빈 방이 없음, 새로운 방 생성...");
        // 접속 상태 표시
        connectionInfoText.text = "빈 방이 없음, 새로운 방 생성...";
        // 최대 2명을 수용 가능한 빈 방을 생성
        PhotonNetwork.CreateRoom(null, new RoomOptions { MaxPlayers = 2 });
    }

    // 상담소에 이동이 완료된 경우 자동 실행
    public override void OnJoinedRoom()
    {
        // 접속 상태 표시
        connectionInfoText.text = "상담소 이동 성공";
        Debug.Log("상담소 이동 성공");

        // 모든 사용자들이 상담소 씬을 로드하게 함
        //PhotonNetwork.LoadLevel("Center_Scene");
        PhotonNetwork.LoadLevel("Counselor_Counsel_Scene");
    }

    // 방 퇴장
    public void OutRoom() 
    {
        PhotonNetwork.Disconnect();
    }
        


}
