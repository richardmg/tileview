import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

import QtQuick3D.TileView

Window {
    id: window
    width: 1024
    height: 640
    visible: true
    color: "#848895"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0.05, 1) }
            GradientStop { position: 1.0; color: Qt.rgba(0.15, 0.15, 0.15, 1) }
        }

        View3D {
            id: mainView
            anchors.fill: parent
            camera: personCamera
            importScene: scene
            environment: SceneEnvironment {
                //            backgroundMode: SceneEnvironment.SkyBox
                //            clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
                //            backgroundMode: SceneEnvironment.Color
                probeOrientation: Qt.vector3d(0, -90, 0)
                lightProbe: Texture {
                    source: "maps/OpenfootageNET_garage-1024.hdr"
                }
            }
        }
    }

    Node {
        id: scene

        TileView {
            id: tileView
            center: personCamera.position
            tileSize: Qt.vector3d(400, 400, 400)
            tileCount: Qt.vector3d(6, 6, 6)

            Connections {
                target: personCamera
                function onRotationChanged()
                {
                    tileView.direction = personCamera.forward
                }
            }

            delegate: Node {
                Star {
                    instancing: randomInstancing
                    SequentialAnimation on eulerRotation {
                        loops: Animation.Infinite
                        PropertyAnimation {
                            duration: Math.random(10000) + 10000
                            from: Qt.vector3d(0, 0, 0)
                            to: Qt.vector3d(360, 360, 360)
                        }
                    }
                }
                RandomInstancing {
                    id: randomInstancing
                    instanceCount: 300
                    randomSeed: 2021

                    position: InstanceRange {
                        from: Qt.vector3d(-200, -200, -200)
                        to: Qt.vector3d(200, 200, 200)
                    }
                }
            }
        }

        PerspectiveCamera {
            id: personCamera
            position: Qt.vector3d(0, 0, 0)
            Model {
                source: "#Cone"
                scale: Qt.vector3d(0.5, 0.5, 0.5)
                eulerRotation: Qt.vector3d(-90, 0, 0)
                materials: [
                    DefaultMaterial {
                        diffuseColor: "red"
                    }
                ]
            }
        }

        DirectionalLight {
            position: Qt.vector3d(500, 500, 500)
            eulerRotation: Qt.vector3d(-70, 45, 0)
            color: Qt.rgba(1.0, 1.0, 1.0, 1.0)
            ambientColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        }

    }

    WasdController {
        controlledObject: personCamera
    }

}
