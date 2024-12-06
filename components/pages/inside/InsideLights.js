import React, { useState } from 'react';
import { View, Text, StyleSheet, ImageBackground } from 'react-native';
import Slider from '@react-native-community/slider';
import Title from '../../Title';
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";
import { bedRoomSlider, guestSlider } from '../../SensorsCommunication';

const CustomSlider = ({navigation}) => {
  const [sliderValueLiving, setSliderValueLiving] = useState(0);
  const [sliderValueBedRoom, setSliderValueBedRoom] = useState(0);
  const [sliderValueGuest, setSliderValueGuest] = useState(0);

  return (

   
    <View style={styles.container}>
 <ImageBackground
    source={require('../../../assets/images/background/InsideLights.png')}
    resizeMode='cover'
    style={styles.backgroundImg}
  >

<View style={styles.title}>
          <Title name="Inside Lights" />
        </View>

        <View style={styles.group}>
          <GroupButtons
            paragraphName="Hall"
            buttonLeftName="On"
            buttonLeftAction="turnOnHallLed"
            buttonRightName="Off"
            buttonRightAction="turnOffHallLed"
          />
        </View>

  <View style={styles.containerLiving}>
        <GroupButtons
          paragraphName="Living"
          buttonLeftName="On"
          buttonLeftAction="turnOnLivingLed"
          buttonRightName="Off"
          buttonRightAction="turnOffLivingLed"

        />
        
      <View style={styles.sliderContainer}>
        <Text style={styles.sliderText}>{`${sliderValueLiving.toFixed(0)}%`}</Text>
        <Slider
          style={styles.slider}
          minimumValue={0}
          maximumValue={100}
          minimumTrackTintColor="#FF5C58"
          maximumTrackTintColor="#FF8C84"
          thumbTintColor="#FF5C58"
          value={sliderValueLiving}
          onValueChange={(value) => setSliderValueLiving(value)}
        />
      </View>
    </View>
    
    {/*BedRoom */}
    <View style={styles.containerLiving}>
    <GroupButtons
          paragraphName="BedRoom"
          buttonLeftName="On"
          buttonLeftAction="turnOnBedRoomLed"
          buttonRightName="Off"
          buttonRightAction="turnOffBedRoomLed"
        />
        
      <View style={styles.sliderContainer}>
        <Text style={styles.sliderText}>{`${sliderValueBedRoom.toFixed(0)}%`}</Text>
        <Slider
          style={styles.slider}
          minimumValue={0}
          maximumValue={100}
          minimumTrackTintColor="#FF5C58"
          maximumTrackTintColor="#FF8C84"
          thumbTintColor="#FF5C58"
          value={sliderValueBedRoom}
          onValueChange={(value) => {
          setSliderValueBedRoom(value); // Actualizează valoarea local
          bedRoomSlider(value.toFixed(0)); // Trimite valoarea către Firebase
        }}    
        />
      </View>
    </View>

{/* Guest */}
    <View style={styles.containerLiving}>
    <GroupButtons
          paragraphName="Guest"
          buttonLeftName="On"
          buttonLeftAction="turnOnGuestLed"
          buttonRightName="Off"
          buttonRightAction="turnOffGuestLed"
        />
        
      <View style={styles.sliderContainer}>
        <Text style={styles.sliderText}>{`${sliderValueGuest.toFixed(0)}%`}</Text>
        <Slider
          style={styles.slider}
          minimumValue={0}
          maximumValue={100}
          minimumTrackTintColor="#FF5C58"
          maximumTrackTintColor="#FF8C84"
          thumbTintColor="#FF5C58"
          value={sliderValueGuest}
          onValueChange={(value) => {
          setSliderValueGuest(value); 
          guestSlider(value.toFixed(0)); 
        }}          />
      </View>
    </View>

    <View style={styles.backButton}>
          <DirectionButton name={"←"} link={"Inside"} navigation={navigation} />
        </View>
      </ImageBackground>
    </View>
  );
};

const styles = StyleSheet.create({
  backgroundImg: {
    flex: 1,
    width: '100%',
    justifyContent: 'center',
    alignItems: 'center',
  },
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    // padding: 20,
    backgroundColor: '#531225',
    opacityColor: 1,
    },
    title: {
      marginTop: 15,
    },
  containerLiving: {
    width: '100%',
    // height: 250,
    // justifyContent:'center',
    alignItems: 'center',
    // marginTop: -30,
    // borderWidth: 1,
    // borderColor: '#C55656',

  },

  sliderContainer: {

    marginVertical: 15,
    alignItems: 'center',
    // justifyContent: 'center',
    borderWidth: 1,
    borderRadius: 30,
    borderColor: '#C55656',
    backgroundColor: 'rgba(85, 17, 17, 0.5)',
    height: 50,
    width: '90%',

  },
  sliderText: {
    marginTop: 7,
    fontSize: 30,
    fontWeight: 'bold',
    color: '#FFDDDD',  
  },
  slider: {
    width: '80%',
    // height: 40,
    top: -14,
    // bottom: 2,
    // position: 'absolute',
  },

  backButton:{
    marginTop: -15,
    
  }
});

export default CustomSlider;
