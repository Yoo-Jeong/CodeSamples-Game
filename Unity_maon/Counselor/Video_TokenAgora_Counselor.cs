using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using agora_gaming_rtc;
using agora_utilities;

using System.Threading.Tasks;
using System;
using Object = UnityEngine.Object;

using Photon.Pun;
using Photon.Realtime;


//토큰서버에서 토큰을 가져온다. !!로컬서버라 나(유정)이 토큰서버를 실행하고 있어야 돌아감!!
public class Video_TokenAgora_Counselor : MonoBehaviour
{
    VideoSurface myView;
    VideoSurface remoteView;

    [SerializeField]
    private string APP_ID = "6b17d6a455dc4642a36f39abbba84659";
    public Text logText;
    private Logger logger;
    private IRtcEngine mRtcEngine = null;
    private const float Offset = 100;
    private static string channelName = "swuniverse";
    private static string channelToken = "";
    //private static string tokenBase = "http://localhost:8080";
    //private static string tokenBase = "http://3Bears.iptime.org:18080";
    private static string tokenBase = "https://maon-server.run.goorm.io";
    private CONNECTION_STATE_TYPE state = CONNECTION_STATE_TYPE.CONNECTION_STATE_DISCONNECTED;
 

    //상담사 캐릭터 애니메이션
    public Animator animator;

    // Use this for initialization
    void Start()
    {

        CheckAppId();
        InitEngine();    
        JoinChannel();


        OwnerTake();
        animator = GetComponent<Animator>();
    }

    

    void RenewOrJoinToken(string newToken)
    {
        Video_TokenAgora_Counselor.channelToken = newToken;
        if (state == CONNECTION_STATE_TYPE.CONNECTION_STATE_DISCONNECTED
            || state == CONNECTION_STATE_TYPE.CONNECTION_STATE_DISCONNECTED
            || state == CONNECTION_STATE_TYPE.CONNECTION_STATE_FAILED
        )
        {
            // 연결되어 있지 않은 경우 채널 참가
            JoinChannel();
        }
        else
        {
            // 이미 연결되어 있는 경우 토큰 업데이트
            UpdateToken();
        }
    }

    

    // Update is called once per frame
    void Update()
    {
        PermissionHelper.RequestMicrophontPermission();
        PermissionHelper.RequestCameraPermission();

       
    }

    void UpdateToken()
    {
        mRtcEngine.RenewToken(Video_TokenAgora_Counselor.channelToken);
    }

    void CheckAppId()
    {
        logger = new Logger(logText);
        logger.DebugAssert(APP_ID.Length > 10, "appID를 입력해주세요.");
    }

    void InitEngine()
    {
        mRtcEngine = IRtcEngine.GetEngine(APP_ID);
        mRtcEngine.SetLogFile("log.txt");
        mRtcEngine.SetChannelProfile(CHANNEL_PROFILE.CHANNEL_PROFILE_LIVE_BROADCASTING);
        mRtcEngine.SetClientRole(CLIENT_ROLE_TYPE.CLIENT_ROLE_BROADCASTER);
        mRtcEngine.EnableAudio();
        mRtcEngine.EnableVideo();
        mRtcEngine.EnableVideoObserver();
        mRtcEngine.OnJoinChannelSuccess += OnJoinChannelSuccessHandler;
        mRtcEngine.OnLeaveChannel += OnLeaveChannelHandler;
        mRtcEngine.OnWarning += OnSDKWarningHandler;
        mRtcEngine.OnError += OnSDKErrorHandler;
        mRtcEngine.OnConnectionLost += OnConnectionLostHandler;
        mRtcEngine.OnUserJoined += OnUserJoinedHandler;
        mRtcEngine.OnUserOffline += OnUserOfflineHandler;

        //Set the token expiration handler
        mRtcEngine.OnTokenPrivilegeWillExpire += OnTokenPrivilegeWillExpireHandler;
        mRtcEngine.OnConnectionStateChanged += OnConnectionStateChangedHandler;


        //마이크 볼륨을 확인하기 위함.
        mRtcEngine.EnableAudioVolumeIndication(200, 3);
        mRtcEngine.OnVolumeIndication += VolumeIndicationHandler;

    }

    void JoinChannel()
    {
        if (string.IsNullOrEmpty(channelToken))
        {
            StartCoroutine(HelperClass.FetchToken(tokenBase, channelName, 1, this.RenewOrJoinToken));
            return;
        }
        mRtcEngine.JoinChannelByKey(channelToken, channelName, "", 1);

       
    }

    void OnJoinChannelSuccessHandler(string channelName, uint uid, int elapsed)
    {
        logger.UpdateLog(string.Format("sdk 버전: ${0}", IRtcEngine.GetSdkVersion()));
        logger.UpdateLog(string.Format("채널입장 성공 channelName: {0}, uid: {1}, elapsed: {2}", channelName, uid, elapsed));
        logger.UpdateLog(string.Format("새로운 Token: {0}", Video_TokenAgora_Counselor.channelToken));
        // HelperClass.FetchToken(tokenBase, channelName, 0, this.RenewOrJoinToken);      

        //상담사 자신의 웹캠 화면 
        GameObject go = GameObject.Find("MyView_RawImage");
        myView = go.AddComponent<VideoSurface>();

    }

    public void Leave()
    {
        Debug.Log("OnApplicationQuit : 아고라 퇴장");
        if (mRtcEngine != null)
        {
            mRtcEngine.LeaveChannel();
            mRtcEngine.DisableVideoObserver();
            IRtcEngine.Destroy();
        }

    }

    void OnLeaveChannelHandler(RtcStats stats)
    {
        logger.UpdateLog("OnLeaveChannelSuccess");
        Debug.Log("상담소 퇴장 완료");
        DestroyVideoView(0);
    }

    //앱이나 에디터가 닫힐 때 인스턴스를 삭제
    void OnApplicationQuit()
    {
        Debug.Log("에디터 닫음  : OnApplicationQuit");
        if (mRtcEngine != null)
        {
            mRtcEngine.LeaveChannel();
            mRtcEngine.DisableVideoObserver();
            IRtcEngine.Destroy();
        }
    }

    void OnUserJoinedHandler(uint uid, int elapsed)
    {
        logger.UpdateLog(string.Format("다른 유저 입장  uid: ${0} elapsed: ${1}", uid, elapsed));
        makeVideoView(uid);
    }

    void OnUserOfflineHandler(uint uid, USER_OFFLINE_REASON reason)
    {
        logger.UpdateLog(string.Format("다른 유저 퇴장 uid: ${0}, reason: ${1}", uid, (int)reason));
        DestroyVideoView(uid);
    }

    void OnTokenPrivilegeWillExpireHandler(string token)
    {
        StartCoroutine(HelperClass.FetchToken(tokenBase, channelName, 0, this.RenewOrJoinToken));
    }

    void OnConnectionStateChangedHandler(CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason)
    {
        this.state = state;
        logger.UpdateLog(string.Format("ConnectionState changed {0}, reason: ${1}", state, reason));
    }

    void OnSDKWarningHandler(int warn, string msg)
    {
        logger.UpdateLog(string.Format("OnSDKWarning warn: {0}, msg: {1}", warn, msg));
    }

    void OnSDKErrorHandler(int error, string msg)
    {
        logger.UpdateLog(string.Format("OnSDKError error: {0}, msg: {1}", error, msg));
    }

    void OnConnectionLostHandler()
    {
        logger.UpdateLog(string.Format("OnConnectionLost "));
    }


    private void DestroyVideoView(uint uid)
    {
        GameObject go = GameObject.Find(uid.ToString());
        if (!ReferenceEquals(go, null))
        {
            Object.Destroy(go);
        }
    }

    private void makeVideoView(uint uid)
    {
        GameObject go = GameObject.Find("RemoteView_RawImage");

        if (remoteView == null)
        {
            remoteView = go.AddComponent<VideoSurface>();
        }

        remoteView.SetForUser(uid);
        remoteView.SetEnable(true);
        remoteView.SetVideoSurfaceType(AgoraVideoSurfaceType.RawImage);
    }



    //마이크 볼륨을 확인해서 말하는 중인지를 알려주는 함수
    protected void VolumeIndicationHandler(AudioVolumeInfo[] speakers, int speakerNumber, int totalVolume)
    {
        if (speakerNumber == 0 || speakers == null)
        {
            //Debug.Log(string.Format("onVolumeIndication only local {0}", totalVolume));
        }

        for (int idx = 0; idx < speakerNumber; idx++)
        {
            string volumeIndicationMessage = string.Format("{0} onVolumeIndication {1} {2}",
                speakerNumber, speakers[idx].uid, speakers[idx].volume);
            //Debug.Log(volumeIndicationMessage);

            if (speakerNumber != 1 || speakers[0].uid != 0)
            {
                if (speakers[0].volume > 50)
                {
                    Debug.Log(speakerNumber + " / " + speakers[0].uid
                        + " 내담자가 말하는 중입니다." + "  볼륨 크기: " + speakers[0].volume);
                    //PlayTalk();
                }
                else
                {
                    //Debug.Log(speakerNumber + " / " + speakers[0].uid + " 상담사가 말하는 중이 아닙니다.");
                    //StopTalk();

                }
            }
        }
    }



    //상담사캐릭터 애니메이션관련 시작
    //포톤 마스터 권한
    public void OwnerTake()
    {
        if (this.GetComponent<PhotonView>().Owner.ActorNumber == PhotonNetwork.LocalPlayer.ActorNumber)
        {
            Debug.Log("소유자 입니다.");
        }
        else
        {
            this.GetComponent<PhotonView>().TransferOwnership(PhotonNetwork.LocalPlayer.ActorNumber);
            Debug.Log("소유권을 가져옵니다.");
            
        }

        for (int i = 0; i >= PhotonNetwork.PlayerList.Length; i++)
        {
            Debug.Log(PhotonNetwork.PlayerList[i] + " / " + PhotonNetwork.PlayerList[i].ActorNumber);
        }

    }


  /*  public void ClickNod()
    {
        // PlayNod()를 모든 클라이언트가 볼 수 있도록 Pun RPC이용해 호출
        photonView.RPC("PlayNod", RpcTarget.All);
    }
*/
    public void PlayNod()
    {

        Debug.Log("그렇군요 애니메이션 플레이");
        //animator.SetTrigger("Nod");  //Trigger 동기화가 느려서 bool로 대체       
        animator.SetBool("nod", true);
        Invoke("StopNod", 0.5f);
    }

    public void StopNod()
    {
        //Debug.Log("그렇군요 애니메이션 중단");
        animator.SetBool("nod", false);
    }


    public void PlayThumbup()
    {
        Debug.Log("잘하셨어요 애니메이션 플레이");
        animator.SetBool("thumbup", true);
        Invoke("StopThumbup", 0.2f);
    }


    public void StopThumbup()
    {
        //Debug.Log("잘하셨어요 애니메이션 중단");
        animator.SetBool("thumbup", false);
    }


    public void PlayFace_sad()
    {
        Debug.Log("속상해요 애니메이션 플레이");
        animator.SetBool("face_sad", true);
        Invoke("StopFace_sad", 0.2f);
    }

    public void StopFace_sad()
    {
        //Debug.Log("속상해요 애니메이션 중단");
        animator.SetBool("face_sad", false);
    }


    public void PlayTalk()
    {
        Debug.Log("Talk 애니메이션 플레이");
        animator.SetBool("talk", true);
        Invoke("StopTalk", 0.2f);
    }

    public void StopTalk()
    {
        animator.SetBool("talk", false);
    }






}