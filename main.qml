import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

import TileEngine

Window {
    id: window
    width: 1280
    height: 720
    visible: true
    color: "#848895"

    TileEngine {

    }

    View3D {
        id: v3d
        anchors.fill: parent
        camera: camera

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 0, 600)
        }

        DirectionalLight {
            position: Qt.vector3d(-500, 500, -100)
            color: Qt.rgba(0.4, 0.2, 0.6, 1.0)
            ambientColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
        }

        PointLight {
            position: Qt.vector3d(0, 0, 100)
            color: Qt.rgba(0.1, 1.0, 0.1, 1.0)
            ambientColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        }

        Model {
            scale: Qt.vector3d(100, 100, 100)
            geometry: ExampleTriangleGeometry {
                uv: true
            }
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
        controlledObject: camera
    }

}
