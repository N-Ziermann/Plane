package com.z.plane

import android.Manifest
import android.content.pm.PackageManager
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.view.View
import android.widget.SeekBar
import android.widget.TextView
import androidx.core.content.ContextCompat
import androidx.core.app.ActivityCompat
import org.w3c.dom.Text
import java.lang.Exception
import java.util.*
import kotlin.collections.ArrayList

class MainActivity : AppCompatActivity() {

    var adapter: BluetoothAdapter? = null // ? means it can be null
    var debugMsg:TextView? = null
    var pairedDevices: ArrayList<BluetoothDevice> = ArrayList()
    var activeDeviceId = 0
    var activeDevice: BluetoothDevice? = null
    var permissionsNeeded: Array<String> = arrayOf(Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN)
    var permissionCode: Int = 1 // can be any number

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        //// get elements from screen
        debugMsg = findViewById<TextView>(R.id.testText)
        val reloadButton = findViewById<Button>(R.id.reloadButton)
        val nextButton = findViewById<Button>(R.id.nextDeviceButton)
        val prevButton = findViewById<Button>(R.id.lastDeviceButton)
        val connectButton = findViewById<Button>(R.id.connectButton)
        var connectButtonInsec = findViewById<Button>(R.id.connectButtonInsec)


        // get permissions:
        for (perm in permissionsNeeded){
            if(ContextCompat.checkSelfPermission(this, perm) != PackageManager.PERMISSION_GRANTED)  //if permission missing
                ActivityCompat.requestPermissions(this, arrayOf<String>(perm), permissionCode)
        }

        //// get bluetooth adapter
        adapter = BluetoothAdapter.getDefaultAdapter()

        //// check if bluetooth works the way it should
        if(adapter == null){
            debugMsg!!.text = "Bluetooth not supported"
            return
        }
        else{
            debugMsg!!.text = "Bluetooth works"
        }


        if(!adapter!!.isEnabled){           // turn on bluetooth
            val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetoothIntent, 1)
        }

        // first check for devices:
        loadPairedDevices()

        // assign buttons to tasks
        reloadButton.setOnClickListener{    // check if new bluetooth connections were established
            loadPairedDevices()
        }

        nextButton.setOnClickListener{      // go forwards 1 device in list of bluetooth devices
            if(!pairedDevices.isEmpty()) {
                activeDeviceId += 1
                if (pairedDevices.lastIndex < activeDeviceId) {
                    activeDeviceId = 0
                }
                activateDevice()
            }
        }

        prevButton.setOnClickListener{      // go back 1 device in list of bluetooth devices
            if(!pairedDevices.isEmpty()) {
                activeDeviceId -= 1
                if (0 > activeDeviceId) {
                    activeDeviceId = pairedDevices.lastIndex
                }
                activateDevice()
            }
        }

        connectButtonInsec.setOnClickListener{
            // connect to bluetooth device
            var socket: BluetoothSocket = activeDevice!!.createInsecureRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))

            //adapter!!.cancelDiscovery() // saves performance
            Log.println(Log.ERROR,"i","i")
            try{
                socket.connect()

                //val speedBar = findViewById<SeekBar>(R.id.speedBar)
                //socket.outputStream.write(speedBar.progress)
            }
            catch (e: Exception){
                debugMsg!!.text = "" + e
                Log.println(Log.ERROR,""+e,""+e)
            }
        }

        connectButton.setOnClickListener{
            // connect to bluetooth device
            var socket: BluetoothSocket = activeDevice!!.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))

            //adapter!!.cancelDiscovery() // saves performance

            try{
                if(!socket!!.isConnected)
                    socket.connect()
                else
                    Log.println(Log.ERROR,"already con","already con")
                //val speedBar = findViewById<SeekBar>(R.id.speedBar)
                //socket.outputStream.write(speedBar.progress)
            }
            catch (e: Exception){
                debugMsg!!.text = "" + e
                Log.println(Log.ERROR,""+e,""+e)
            }
        }

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
            debugMsg!!.text = "No devices found"
        }
    }

    fun activateDevice(){
        activeDevice = pairedDevices[activeDeviceId]
        debugMsg!!.text = activeDevice!!.getName() + "\n" + activeDevice
    }


}
