import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

import TileEngine

Window {
    id: window
    width: 1024
    height: 640
    visible: true
    color: "#848895"

    View3D {
        id: mainView
        anchors.fill: parent
        camera: personCamera
        importScene: scene
        environment: SceneEnvironment {
//            backgroundMode: SceneEnvironment.SkyBox
            clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
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
        height: 150
        width: 250
        camera: droneCamera
        importScene: scene
        environment: SceneEnvironment {
            clearColor: Qt.rgba(0.8, 0.8, 0.8, 1.0)
            backgroundMode: SceneEnvironment.Color
        }
    }

    Node {
        id: scene

//        TileView {
//            center: personCamera.position
//            tileSize: Qt.vector3d(50, 50, 50)
//            tileCount: Qt.vector3d(6, 6, 6)

//            delegate: Asteroid {
//                SequentialAnimation on eulerRotation {
//                    loops: Animation.Infinite
//                    PropertyAnimation {
//                        duration: Math.random(10000) + 10000
//                        from: Qt.vector3d(0, 0, 0)
//                        to: Qt.vector3d(360, 360, 360)
//                    }
//                }
//            }
//        }

        TileView {
            id: tileView
            center: personCamera.position
            tileSize: Qt.vector3d(500, 500, 500)
            tileCount: Qt.vector3d(2, 2, 2)

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
                    instanceCount: 500
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

        OrthographicCamera {
            id: droneCamera
            position: Qt.vector3d(personCamera.x, personCamera.y + 1000, personCamera.z)
            eulerRotation: Qt.vector3d(-90, 0, 0)
            horizontalMagnification: 0.1
            verticalMagnification: 0.1
        }

        DirectionalLight {
            position: Qt.vector3d(500, 500, 500)
            eulerRotation: Qt.vector3d(-70, 45, 0)
            color: Qt.rgba(1.0, 1.0, 1.0, 1.0)
            ambientColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        }

        Model {
            scale: Qt.vector3d(0.1, 0.1, 0.1)
            source: "#Cube"
//            instancing: randomInstancing
//            geometry: ExampleTriangleGeometry { uv: true }
            materials: [
                DefaultMaterial {
                    Texture {
                        id: baseColorMap
                        source: "qt_logo_rect.png"
                    }
                    cullMode: DefaultMaterial.NoCulling
                    diffuseMap: baseColorMap
//                    specularAmount: 0.5
//                    lineWidth: 3
                }
            ]
        }

    }

    WasdController {
        controlledObject: personCamera
    }

}
