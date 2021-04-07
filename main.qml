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
            clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
            backgroundMode: SceneEnvironment.Color
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
            clearColor: Qt.rgba(0.3, 0.3, 0.3, 1.0)
            backgroundMode: SceneEnvironment.Color
        }
    }

    Node {
        id: scene

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

//                PointLight {
//                    color: Qt.rgba(1.0, 0.0, 0.0, 1.0)
////                    ambientColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
//                }
            }
        }

        OrthographicCamera {
            id: droneCamera
            position: Qt.vector3d(personCamera.x, personCamera.y + 1000, personCamera.z)
            eulerRotation: Qt.vector3d(-90, 0, 0)
            horizontalMagnification: 0.2
            verticalMagnification: 0.2
        }

        TileView {
            center: personCamera.position
            tileSize: Qt.vector3d(80, 80, 80)
            tileCount: Qt.vector3d(8, 8, 8)

            delegate: Model {
                source: "#Cube"
                scale: Qt.vector3d(0.05, 0.05, 0.05)
                materials: [
                    DefaultMaterial {
                        diffuseColor: "yellow"
                    }
                ]
//                TileView.onTileChanged: print("new tile pos:", TileView.tile)
            }
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
