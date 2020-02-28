package com.z.plane

import android.Manifest
import android.content.pm.PackageManager
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.hardware.SensorManager
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.view.View
import android.widget.EditText
import android.widget.SeekBar
import android.widget.TextView
import androidx.core.content.ContextCompat
import androidx.core.app.ActivityCompat
import org.w3c.dom.Text
import java.lang.Exception
import java.util.*
import kotlin.collections.ArrayList
// setting intervals:
import java.util.Timer
import kotlin.concurrent.schedule
import kotlin.math.absoluteValue


class MainActivity : AppCompatActivity(), SensorEventListener{

    // logic
    var isFlying:Boolean = false

    // UI-Elements
    var deviceText:TextView? = null
    var dataText: TextView? = null
    var speedBar:SeekBar? = null
    var nextButton:Button? = null
    var prevButton:Button? = null
    var connectButton:Button? = null
    var connectButtonInsec:Button? = null
    var recalibrate:Button? = null
    var reloadButton:Button? = null
    var fly:Button? = null
    var intervalField:EditText? = null


    // bluetooth
    var adapter: BluetoothAdapter? = null // ? means it can be null
    var pairedDevices: ArrayList<BluetoothDevice> = ArrayList()
    var activeDeviceId = 0
    var activeDevice: BluetoothDevice? = null
    var permissionsNeeded: Array<String> = arrayOf(Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN)
    var permissionCode: Int = 1     // can be any number
    var sendInterval: Long = 500    // miliseconds between each transmission
    var btSocket: BluetoothSocket? = null

    // gyroscope
    var sensorManager: SensorManager? = null
    var gyroscope:Sensor? = null
    var gyroData:Array<Int> = arrayOf(0,0)
    var gyroThresholdMin = arrayOf(75,40)          // threshold after which the sensors data gets recognized
    var gyroThresholdMax = arrayOf(150,200)         // threshold after which every value will just be an angle of 180


    fun startInterval(){        // will repeatedly try to send data over bluetooth

        if (isFlying) {
            // prepare data

            val speedBarData = speedBar!!.progress                      // motor value (between 0 and 100)
            val sendableGyroData = arrayOf(90, 90)                       // gyro data after applying threshold (between 0 and 180) (90,90 because thats the default)

            if (gyroData[0].absoluteValue >= gyroThresholdMin[0]) {        // vertical axis
                if(gyroData[0].absoluteValue >= gyroThresholdMax[0]) {
                    if (gyroData[0] > 0) {  // go up
                        sendableGyroData[0] = 180
                    } else {               // go down
                        sendableGyroData[0] = 0
                    }
                }else{
                    if(gyroData[0] > 0){
                        sendableGyroData[0] = (90f + (100 * (gyroData[0].toFloat().absoluteValue - gyroThresholdMin[0]) / (gyroThresholdMax[0].toFloat() - gyroThresholdMin[0]))).toInt()
                    }else{
                        sendableGyroData[0] = (90f - (100 * (gyroData[0].toFloat().absoluteValue - gyroThresholdMin[0]) / (gyroThresholdMax[0].toFloat() - gyroThresholdMin[0]))).toInt()
                    }
                }
            }

            if (gyroData[1].absoluteValue >= gyroThresholdMin[1]) {        // horizontal axis
                if(gyroData[1].absoluteValue >= gyroThresholdMax[1]) {
                    if (gyroData[1] > 0) {  // go up
                        sendableGyroData[1] = 180
                    } else {               // go down
                        sendableGyroData[1] = 0
                    }
                }else{
                    if(gyroData[1] > 0){
                        sendableGyroData[1] = (90f + (100 * (gyroData[1].toFloat().absoluteValue - gyroThresholdMin[1]) / (gyroThresholdMax[1].toFloat() - gyroThresholdMin[1]))).toInt()
                    }
                    else{
                        sendableGyroData[1] = (90f - (100 * (gyroData[1].toFloat().absoluteValue - gyroThresholdMin[1]) / (gyroThresholdMax[1].toFloat() - gyroThresholdMin[1]))).toInt()
                    }
                }
            }

            // small bugfixes:
            if(sendableGyroData[1] < 0){
                sendableGyroData[1] = 0
            }
            if(sendableGyroData[1] > 180){
                sendableGyroData[1] = 180
            }
            if(sendableGyroData[0] < 0){
                sendableGyroData[0] = 0
            }
            if(sendableGyroData[0] > 180){
                sendableGyroData[0] = 180
            }



            val dataString = "" + sendableGyroData[1] + "," + sendableGyroData[0] + "," + speedBarData + ";"  //Horizontal(0...180), Vertical(0...180), motorspeed(0...100)
            dataText!!.text = dataString
            //Log.println(Log.DEBUG, dataString, dataString)


            // try to send data
            if(btSocket != null && btSocket!!.isConnected){    // working connection is set up
                try {
                    btSocket!!.outputStream.write(dataString.toByteArray())
                }catch (e:Exception){
                    e.printStackTrace()
                }

            }
        }

        // constantly repeat
        Timer().schedule(sendInterval) {
            startInterval()
        }
    }


    override fun onSensorChanged(event: SensorEvent?) {     // cant be used for sending as interval for receiving sensor data is to irregular
        // amount by which the values changed
        val x = (event!!.values[0]*100).toInt()
        val y = (event!!.values[1]*100).toInt()

        gyroData[0] += x;
        gyroData[1] += y;

    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ///// SENSORS

        // setup sensor
        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager
        gyroscope = sensorManager!!.getDefaultSensor(Sensor.TYPE_GYROSCOPE)
        sensorManager!!.registerListener(this, gyroscope, SensorManager.SENSOR_DELAY_NORMAL)


        //// load elements from screen
        deviceText = findViewById<TextView>(R.id.testText)
        dataText = findViewById<TextView>(R.id.dataText)
        reloadButton = findViewById<Button>(R.id.reloadButton)
        nextButton = findViewById<Button>(R.id.nextDeviceButton)
        prevButton = findViewById<Button>(R.id.lastDeviceButton)
        connectButton = findViewById<Button>(R.id.connectButton)
        connectButtonInsec = findViewById<Button>(R.id.connectButtonInsec)
        recalibrate  = findViewById<Button>(R.id.recalibrate)
        fly = findViewById<Button>(R.id.fly)
        speedBar = findViewById<SeekBar>(R.id.speedBar)
        intervalField = findViewById<EditText>(R.id.interval)

        // load defaults
        intervalField!!.setText(sendInterval.toString())

        ///// BLUETOOTH

        // get permissions:
        for (perm in permissionsNeeded){
            if(ContextCompat.checkSelfPermission(this, perm) != PackageManager.PERMISSION_GRANTED)  //if permission missing
                ActivityCompat.requestPermissions(this, arrayOf<String>(perm), permissionCode)
        }

        //// get bluetooth adapter
        adapter = BluetoothAdapter.getDefaultAdapter()

        //// check if bluetooth works the way it should
        if(adapter == null){
            deviceText!!.text = "Bluetooth not supported"
            return
        }
        else{
            deviceText!!.text = "Bluetooth works"
        }


        if(!adapter!!.isEnabled){           // turn on bluetooth
            val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetoothIntent, 1)
        }


        assignButtons()
        loadPairedDevices()
        startInterval()
    }


    fun loadPairedDevices(){
        var devices = adapter!!.bondedDevices

        if(!devices.isEmpty()) {
            for (device: BluetoothDevice in devices) {
                pairedDevices.add(device)
            }
            activateDevice()
        }
        else{
            deviceText!!.text = "No devices found"
        }
    }


    fun activateDevice(){
        activeDevice = pairedDevices[activeDeviceId]
        deviceText!!.text = activeDevice!!.getName() + "\n" + activeDevice
    }

    fun assignButtons(){
        // assign buttons to tasks

        fly!!.setOnClickListener {
            sendInterval = intervalField!!.text.toString().toLong()
            Log.println(Log.ERROR, intervalField!!.text.toString(), intervalField!!.text.toString())
            isFlying = !isFlying
        }

        recalibrate!!.setOnClickListener{     // reset gyro
            gyroData = arrayOf(0,0)
        }

        reloadButton!!.setOnClickListener{    // check if new bluetooth connections were established
            loadPairedDevices()
        }

        nextButton!!.setOnClickListener{      // go forwards 1 device in list of bluetooth devices
            if(!pairedDevices.isEmpty()) {
                activeDeviceId += 1
                if (pairedDevices.lastIndex < activeDeviceId) {
                    activeDeviceId = 0
                }
                activateDevice()
            }
        }

        prevButton!!.setOnClickListener{      // go back 1 device in list of bluetooth devices
            if(!pairedDevices.isEmpty()) {
                activeDeviceId -= 1
                if (0 > activeDeviceId) {
                    activeDeviceId = pairedDevices.lastIndex
                }
                activateDevice()
            }
        }

        connectButtonInsec!!.setOnClickListener{
            // connect to bluetooth device
            var socket: BluetoothSocket = activeDevice!!.createInsecureRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))

            //adapter!!.cancelDiscovery() // saves performance
            Log.println(Log.ERROR,"i","i")
            try{
                socket.connect()

                //socket.outputStream.write(speedBar.progress)
            }
            catch (e: Exception){
                deviceText!!.text = "" + e
                Log.println(Log.ERROR,""+e,""+e)
            }
        }

        connectButton!!.setOnClickListener{
            // connect to bluetooth device
            if(btSocket == null) {
                btSocket = activeDevice!!.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
            }
            if(btSocket!!.isConnected == false){ // no connection yet
                try {
                    if (!btSocket!!.isConnected)
                        btSocket!!.connect()
                    else
                        Log.println(Log.ERROR, "already con", "already con")
                    //socket.outputStream.write()
                } catch (e: Exception) {
                    deviceText!!.text = "" + e
                    Log.println(Log.ERROR, "" + e, "" + e)
                }
            }
        }
    }


    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // just needed to implement SensorEventListener
    }
}
