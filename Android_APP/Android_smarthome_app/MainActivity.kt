package com.example.smarthome

import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.example.mlib.Notification
import com.example.mlib.net.Mqtt
import kotlinx.android.synthetic.main.activity_main.*
import org.eclipse.paho.client.mqttv3.MqttMessage
import org.json.JSONObject
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL
import kotlin.concurrent.thread


val SUB_TOPIC = arrayOf<String>(
    "iot/face/#"
)
const val PUB_TOPIC = "iot/control/servo"
const val SERVER_URI = "tcp://172.30.1.107:1883"

class MainActivity : AppCompatActivity() {

    val TAG = javaClass.simpleName
    lateinit var temp : String

    lateinit var txt_light : String
    lateinit var txt_fire : String
    lateinit var txt_munzi : String

    // private val mRequestAddress = "http://192.168.35.112:8000/sensors/inform/"
    private val mRequestAddress = "http://172.30.1.107:8000/sensors/inform/"


    lateinit var mqttClient: Mqtt

    companion object {
        const val CHANNEL_ID = "com.example.basic_3_mqtt_noti"
        const val CHANNEL_NAME = "택배 서비스"
        const val CHANNEL_DESCRIPTION = "택배가 도착했습니다."
        const val NOTIFICATION_REQUEST = 0
        const val NOTIFICATION_ID = 100
    }


    fun onReceived(topic: String, message: MqttMessage) {
        // 토픽 수신 처리
        val msg = String(message.payload)
        Log.i(TAG,"$topic:$msg")

        val noti = Notification(this)
        noti.createNotificationChannel(CHANNEL_ID, CHANNEL_NAME,
            CHANNEL_DESCRIPTION)
        val pendingIntent = noti.getPendingIntent(
            MainActivity::class.java, NOTIFICATION_REQUEST)
        noti.notifyBasic(CHANNEL_ID, NOTIFICATION_ID,
            "택배알림이", "$message",
            R.drawable.ic_launcher_foreground, pendingIntent)
    }


    fun networking(urlString: String) {
        thread(start=true) {
            // 네트워킹 예외처리를 위한 try ~ catch 문
            try {
                val url = URL(urlString)

                // 서버와의 연결 생성
                val urlConnection = url.openConnection() as HttpURLConnection
                urlConnection.requestMethod = "GET"

                if (urlConnection.responseCode == HttpURLConnection.HTTP_OK) {
                    // 데이터 읽기
                    val streamReader = InputStreamReader(urlConnection.inputStream)
                    val buffered = BufferedReader(streamReader)

                    val content = StringBuilder()
                    while(true) {
                        val line = buffered.readLine() ?: break
                        content.append(line)
                        temp = line
                    }
                    // 스트림과 커넥션 해제
                    val jObject = JSONObject(temp)
                    if (jObject.getString("light") == "1"){
                        txt_light = "꺼짐"
                    }
                    else{
                        txt_light = "켜짐"
                    }

                    if (jObject.getString("fire") == "0"){
                        txt_fire = "안전"
                    }
                    else{
                        txt_fire = "화재 위험"
                    }

                    if (jObject.getString("munzi") == "Good"){
                        txt_munzi = "보통"
                    }
                    else{
                        txt_munzi = "나쁨"
                    }
                    buffered.close()
                    urlConnection.disconnect()
                    runOnUiThread {
                        // UI 작업
                        txt_info_light.text = txt_light
                        txt_info_fire.text = txt_fire
                        txt_info_munzi.text = txt_munzi
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        mqttClient = Mqtt(this, SERVER_URI)
        try {
            // mqttClient.setCallback { topic, message ->}
            mqttClient.setCallback(::onReceived)
            mqttClient.connect(SUB_TOPIC)
        } catch (e: Exception) {
            e.printStackTrace()
        }

        networking(mRequestAddress)

        btn_refresh.setOnClickListener {
            networking(mRequestAddress)
        }

        sw_control.setOnCheckedChangeListener{ _,ischeked ->
            if(ischeked){
                Log.d("sw!!!!!","on")
                mqttClient.publish(PUB_TOPIC,"0")
            }else{
                Log.d("sw!!!!!","off")
                mqttClient.publish(PUB_TOPIC,"1")

            }

        }
    }

}
