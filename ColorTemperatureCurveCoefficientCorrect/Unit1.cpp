//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FileListBox1Click(TObject *Sender)
{
	TabControl1Change( Sender );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TabControl1Change(TObject *Sender)
{
	
	switch( TabControl1 -> TabIndex )
	{
		
		case 0:
		{
			
			char MeanRGBFile[ 150 ];
			
			char LogRGBFile[ 150 ];
			
			strcpy( MeanRGBFile, FileListBox1 -> FileName.c_str() );
			
			strcpy( LogRGBFile, MeanRGBFile );
			
			sprintf( strrchr( MeanRGBFile, '.' ), ".emf" );
			
			sprintf( strrchr( LogRGBFile, '.' ), "_log.txt" );
			
			
			if( !FileExists( MeanRGBFile ) )
			{
				
				FILE *MeanRGBData = fopen( FileListBox1 -> FileName.c_str(), "r" );
				
				FILE *LogRGBData = fopen( LogRGBFile, "w" );
				
				float RValue;
				float GValue;
				float BValue;
				
				float GRRatio;
				float GBRatio;
				
				float LogGRRatio;
				float LogGBRatio;
				
				// assume maximun count is 1000
				float X[ 1000 ];
				float Y[ 1000 ];
				
				float XTotal = 0;
				float YTotal = 0;
				
				float XAverage;
				float YAverage;
				
				float XMin = 10000;
				float XMax = 0;
				
				float YMin = 10000;
				float YMax = 0;
				
				int count = 0;
				
				do
				{
					
					fscanf( MeanRGBData, "%f%f%f", &RValue, &GValue, &BValue );
					
					GRRatio = GValue / RValue;
					GBRatio = GValue / BValue;
					
					LogGRRatio = log10( GRRatio );
					LogGBRatio = log10( GBRatio );
					
					fprintf( LogRGBData, "%f\t%f\n", LogGRRatio, LogGBRatio );
					
					X[ count ] = LogGRRatio;
					Y[ count ] = LogGBRatio;
					
					if( XMin > X[ count ] )
						XMin = X[ count ];
					else if( X[ count ] > XMax )
						XMax = X[ count ];
					
					if( YMin > Y[ count ] )
						YMin = Y[ count ];
					else if( Y[ count ] > YMax )
						YMax = Y[ count ];
					
					XTotal += X[ count ];
					YTotal += Y[ count ];
					
					count ++;
					
				}while( !feof( MeanRGBData ) );
				
				fclose( MeanRGBData );
				fclose( LogRGBData );
				
				XAverage = XTotal / count;
				YAverage = YTotal / count;
				
				float BetaUpper = 0;
				float BetaLower = 0;
				float Beta;
				float Alpha;
				
				for( int i = 0; i < count; i ++ )
				{
					
					BetaUpper += ( X[ i ] - XAverage ) * ( Y[ i ] - YAverage );
					BetaLower += ( X[ i ] - XAverage ) * ( X[ i ] - XAverage );
					
				}
				
				Beta = BetaUpper / BetaLower;
				Alpha = YAverage - Beta * XAverage;
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ %f : %f ]\n", XMin - 0.05, XMax + 0.05 );
				fprintf( fp_domain_script, "set yrange [ %f : %f ]\n", YMin - 0.05, YMax + 0.05 );
				
				fprintf( fp_domain_script, "set xlabel 'Log( G / R )'\n" );
				fprintf( fp_domain_script, "set ylabel 'Log( G / B )'\n" );
				
				// New Lctc : y = Beta * x + Alpha
				float xLeft = XMin - 0.03;			// adjustment for data in order to beautify graph
				float xRight = XMax + 0.03;			// adjustment for data in order to beautify graph
				
				float yLeft = Beta * xLeft + Alpha;
				float yRight = Beta * xRight + Alpha;
				
				fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f nohead\n", xLeft, yLeft, xRight, yRight );
				
				//fprintf( fp_domain_script, "set title '%d photos color ratio in the log domain'\n", count );
				fprintf( fp_domain_script, "set grid\n" );
				fprintf( fp_domain_script, "set pointsize 3\n" );
				fprintf( fp_domain_script, "set output '%s'\n", MeanRGBFile );
				
				fprintf( fp_domain_script, "plot '%s' title 'Lctc : y = %fx + %f' with points 1 6\n", LogRGBFile, Beta, Alpha );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "histogram.dem" );
				remove( LogRGBFile );
				
			}
			
			Image4 -> Picture -> LoadFromFile( MeanRGBFile );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
		}
		
	}
}
//---------------------------------------------------------------------------

