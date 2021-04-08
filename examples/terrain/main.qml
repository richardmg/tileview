import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

import QtQuick3D.TileView
import LandTile

Window {
    id: window
    width: 1024
    height: 640
    visible: true
    color: "#848895"

    Column {
        z: 100
        Slider {
            id: resolutionSlider
            width: 200
            from: 1
            to: 60
            value: 30
            onValueChanged: print("resolution:", value)
        }
        Slider {
            id: sampleSlider
            width: 200
            from: 0.0001
            to: 0.05
            value: 0.001
            onValueChanged: print("sample:", value)
        }
        Slider {
            id: scaleSlider
            width: 200
            from: 1
            to: 300
            value: 175
            onValueChanged: print("scale:", value)
        }
    }

    View3D {
        id: mainView
        anchors.fill: parent
        camera: personCamera
        importScene: scene
        environment: SceneEnvironment {
//            backgroundMode: SceneEnvironment.SkyBox
            clearColor: Qt.rgba(0.6, 0.6, 0.9, 1.0)
            backgroundMode: SceneEnvironment.Color
            probeOrientation: Qt.vector3d(0, -90, 0)
            lightProbe: Texture {
                source: "maps/OpenfootageNET_garage-1024.hdr"
            }
        }
    }

    View3D {
        id: droneView
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        height: 250
        width: 250
        camera: droneCamera
        importScene: scene
        environment: SceneEnvironment {
            clearColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
            backgroundMode: SceneEnvironment.Color
        }
    }

    Node {
        id: scene

        TileView {
            id: tileView
            center: personCamera.position
            tileSize: Qt.vector3d(1000, 1, 1000)
            tileCount: Qt.vector3d(20, 1, 20)

            delegate: Model {
                id: delegate
                scale: Qt.vector3d(1, delegate.parent.scaleSlider.value, 1);
                materials: [ DefaultMaterial { diffuseColor: "green" } ]
                geometry: LandTile {
                    resolution: Qt.vector3d(delegate.parent.resolutionSlider.value, delegate.parent.resolutionSlider.value, delegate.parent.resolutionSlider.value)
                    sampleScale: Qt.vector3d(delegate.parent.sampleSlider.value, delegate.parent.sampleSlider.value, delegate.parent.sampleSlider.value);
                    tileSize: delegate.parent.tileSize
                    position: delegate.position
                }
            }

            property alias resolutionSlider: resolutionSlider
            property alias sampleSlider: sampleSlider
            property alias scaleSlider: scaleSlider

//            Connections {
//                target: personCamera
//                function onRotationChanged() { tileView.direction = personCamera.forward }
//            }
//            Component.onCompleted: direction = personCamera.forward
        }

        PerspectiveCamera {
            id: personCamera
            position: Qt.vector3d(0, 100, 0)
            Model {
                source: "#Cone"
                scale: Qt.vector3d(2.0, 2.0, 2.0)
                eulerRotation: Qt.vector3d(-90, 0, 0)
                materials: [
                    DefaultMaterial {
                        diffuseColor: "red"
                    }
                ]
            }
        }

        OrthographicCamera {
            id: droneCamera
            position: Qt.vector3d(personCamera.x, personCamera.y + 1000, personCamera.z)
            eulerRotation: Qt.vector3d(-90, 0, 0)
            horizontalMagnification: 0.1
            verticalMagnification: 0.1
        }

        DirectionalLight {
            position: Qt.vector3d(500, 500, 0)
            eulerRotation: Qt.vector3d(-45, 0, 0)
            color: Qt.rgba(1.0, 1.0, 1.0, 1.0)
            ambientColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        }

    }

    WasdController {
        controlledObject: personCamera
    }

}