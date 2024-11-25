# Adobe® Photoshop® Connection SDK

## Welcome

Welcome to the Adobe® Photoshop® Connection SDK.

Imagine a creative environment in which the fun of touch, the potential of mobility, and the power of Adobe Photoshop software meet. The Photoshop Connection SDK allows applications on Android™, Blackberry, and iOS mobile and tablet devices, as well as Mac OS and Windows® operating systems, to interact with Photoshop CS5.

For developers, we believe that connecting devices and applications to Photoshop offers you new business opportunities by enabling you to deliver greater value to your existing customers, and helps you to acquire new customers by offering new benefits that harness the power of Photoshop in a way that was impossible before.

With this SDK, developers can create and enable apps to drive and/or communicate with Photoshop CS5 (version 12.0.4 or later) via a TCP connection. It’s now possible for developers to create an ebook of Photoshop tutorials that allows users to drive an Action within Photoshop CS5 from within the eBook. Or, if you find yourself inspired by an ocean view, use a tablet device to mix the colors you see and send those colors directly back to Photoshop CS5 without ever leaving the beach. There are so many possibilities, and with the Photoshop Connection SDK, Adobe is opening Photoshop up to the community to bring these, and other scenarios we haven’t even imagined yet to life.

This SDK delivers a completely new way for people to enhance, transform and enjoy the billions of images captured on devices of all types. We can’t wait to see what you create.

The Photoshop Team

## Introduction

Adobe® Photoshop® CS5 includes new technology that allows an application on a mobile device to control Photoshop. This SDK provides documentation and sample projects that detail the process of creating such an application. The Adobe Photoshop interface can sometimes be overwhelming for both new and seasoned users. With the Adobe Photoshop Connection SDK, developers can now create highly customized applications that can take control of Adobe Photoshop CS5. The potential to streamline or enhance interaction with the application's interface is now limited only by the creativity of the developer community.

Please Note: Adobe® Photoshop® CS5 users that have upgraded to the 12.0.4 release also have access to this feature.

Control in this case is accomplished through configuring Photoshop as a server. Once this is done, a client application can connect to Photoshop via any software program that can communicate over a TCP connection. A TCP connection is typically used via a language that has a socket interface. The Java Socket class found in java.net.Socket is one example of establishing a client-side TCP socket. iOS and Cocoa developers can use the NSNetServiceBrowser or CFStreamCreatePairWithSocketToHost for establishing a connection to Photoshop.

Please see the [How Does it Work](connectionsdk/pages/howDoesItWork.md)? section of this SDK for more information regarding the content of messages delivered over the TCP connection.

This SDK will not teach you how to develop applications for your device; it provides details on how to set up and use Photoshop as a server. For information on how to build applications for the device you are targeting, please consult the SDK provided for that device. See the [requirements](connectionsdk/pages/requirements.md) page for links.

There are currently three Adobe Photoshop Connection Apps available at the Apple App Store. Adobe Nav shows the current tool and the current open documents on a device, as well as allowing users to select tools and change the active document from that same device. Adobe Color Lava provides users a new way to create colors, which can then be sent to Photoshop as swatches. Adobe Eazel is a unique tool for painting with water colors. Images created in Eazel can also be sent to Photoshop.

## [TO BE COMPLETED]

