//
//  MainViewController.swift
//  LightSaber
//
//  Created by Rui Ying on 2019/10/26.
//  Copyright © 2019 Rui Ying. All rights reserved.
//

import CoreBluetooth
import FlexColorPicker
import UIKit

class MainViewController: CustomColorPickerViewController, ColorPickerDelegate, CBCentralManagerDelegate, CBPeripheralDelegate {
    private let serviceUUID = CBUUID(string: "FFE0")
    private let characteristicUUID = CBUUID(string: "FFE1")

    @IBOutlet var aboutButton: UIButton!
    @IBOutlet var activityIndicator: UIActivityIndicatorView!
    @IBOutlet var label: UILabel!
    @IBOutlet var lightSaberImageView: UIImageView!
    @IBOutlet var radialPaletteControl: RadialPaletteControl!

    private var pickedColor: UIColor?

    private var centralManager: CBCentralManager?
    private var peripheral: CBPeripheral?
    private var characteristic: CBCharacteristic?

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)

        lightSaberImageView.tintColor = UIColor.systemPink
        radialPaletteControl.selectedColor = UIColor.systemPink
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        delegate = self
        colorPicker.addControl(radialPaletteControl)

        centralManager = CBCentralManager(delegate: self, queue: .main)
    }

    func colorToString(color: UIColor) -> String {
        var r: CGFloat = 0
        var g: CGFloat = 0
        var b: CGFloat = 0
        color.getRed(&r, green: &g, blue: &b, alpha: nil)

        return String(format: "@%02X%02X%02X$", Int(r * 0xFF),
                      Int(g * 0xFF),
                      Int(b * 0xFF))
    }

    func send(str: String) {
        if characteristic == nil {
            return
        }
        guard let data = str.data(using: String.Encoding.ascii) else { return }
        peripheral?.writeValue(data, for: characteristic!, type: .withoutResponse)
    }

    // Select color
    func colorPicker(_: ColorPickerController, selectedColor: UIColor, usingControl _: ColorControl) {
        pickedColor = selectedColor
        lightSaberImageView.tintColor = selectedColor

        send(str: colorToString(color: selectedColor))
    }

    func colorPicker(_: ColorPickerController, confirmedColor _: UIColor, usingControl _: ColorControl) {}

    // Scan
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            activityIndicator.startAnimating()
            central.scanForPeripherals(withServices: nil, options: nil)
            print("Scanning")
        default:
            label.text = "Disconnected"
            print("Bluetooth unavailable")
        }
    }

    // Connect
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData _: [String: Any], rssi _: NSNumber) {
        if peripheral.name == "LightSaber0" {
            label.text = "Connecting"
            print("Connecting")
            self.peripheral = peripheral
            central.connect(peripheral, options: nil)
        }
    }

    // Discover services
    func centralManager(_: CBCentralManager, didConnect peripheral: CBPeripheral) {
        centralManager?.stopScan()
        label.text = "LightSaber0"
        print("Connected")
        activityIndicator.stopAnimating()

        peripheral.delegate = self
        peripheral.discoverServices([serviceUUID])
        print("Discovering services")
    }

    // Fail to connect
    func centralManager(_: CBCentralManager, didFailToConnect _: CBPeripheral, error _: Error?) {
        label.text = "Disconnected"
        print("Failed to connect")
        activityIndicator.stopAnimating()
    }

    // Reconnect
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error _: Error?) {
        label.text = "Reconnecting"
        print("Reconnecting")
        activityIndicator.startAnimating()
        central.connect(peripheral, options: nil)
    }

    // Discover characteristics
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices _: Error?) {
        guard let service = peripheral.services?[0] else {
            label.text = "Disconnected"
            print("Service unavailable")
            activityIndicator.stopAnimating()
            return
        }

        peripheral.discoverCharacteristics([characteristicUUID], for: service)
        print("Discovering characteristics")
    }

    // Get characteristic
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error _: Error?) {
        characteristic = service.characteristics?.last

        if let characteristic = characteristic {
            peripheral.setNotifyValue(true, for: characteristic)
            print("Setting up notification")
        } else {
            label.text = "Disconnected"
            print("Cannot setup notification")
            activityIndicator.stopAnimating()
        }
    }

    // Notification state update
    func peripheral(_: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        if error != nil {
            print("Failed to setup notification")
        } else if characteristic.isNotifying {
            print("Successfully setup notification")
        } else {
            print("Cancel notification")
        }
        activityIndicator.stopAnimating()
    }

    // Written
    func peripheral(_: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error _: Error?) {
        if let data = characteristic.value {
            let str = String(data: data, encoding: String.Encoding.ascii)
            print("Sent: " + str!)
        }
    }

    // Read
    func peripheral(_: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error _: Error?) {
        if let data = characteristic.value {
            let str = String(data: data, encoding: String.Encoding.ascii)
            print("Received: " + str!)
        }
    }
}
