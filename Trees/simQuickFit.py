#! /usr/bin/env python

from optparse import OptionParser

parser = OptionParser()
parser.add_option('--pt', dest='pt', default=4, type='float',
                  help='pt to cut at')
parser.add_option('--params', '-p', default='nomSimFitf2f3.txt', dest='paramfile',
                  help='initial parameters file')
parser.add_option('--build', default='buildSimPdf_yields.cc', dest='buildfile',
                  help='.cc file')
parser.add_option('--bkgd_PbPb', default=2, type='int', dest='bkgd_PbPb',
                  help='select pdf for PbPb background')
parser.add_option('--bkgd_pp', default=3 , type='int', dest='bkgd_pp',
                  help='select pdf for pp background')
parser.add_option('--trkRot', action='store_true', default=False, dest='trkRot',
                  help='use track rotation for background')
parser.add_option('-b', action='store_true', default=False, dest='b',
                  help='no x windows')
parser.add_option('--ymin', dest='ymin', default=0.0, type='float', help='min rapidity')

parser.add_option('--ymax', dest='ymax', default=2.4, type='float', help='max rapidity')

parser.add_option('--file', default='output.txt', dest='outputfile', help='.txt output')
(opts, args) = parser.parse_args()

import pyroot_logon
from ROOT import gROOT
gROOT.ProcessLine('.L '+opts.buildfile+'+')

#change date everytime date changes (y,m,d)
import os
date = '20130108'
#when new dir msut be created / 1st thing in the day
#os.mkdir('%s' % date)

from ROOT import readData,computeRatio,computeRatioError,buildPdf,\
     mmin,mmax,\
     RooWorkspace,RooFit, RooStats,TCanvas,kBlue,kRed,kGreen,kDashed,buildSimPdf,RooArgSet,\
     RooRealVar,RooMsgService, TMath, TFile

RooMsgService.instance().setGlobalKillBelow(RooFit.WARNING)

hidatafile = 'dimuonTree_2011_hi.root'
ppdatafile = 'dimuonTree_20121912_pp.root'
#ppdatafile = '/home/zhenhu/dimuonTree_pp_7TeV_8pb.root'
#ppdatafile = '../MassTree_NewCuts_pp_HIrereco.root'

cuts = '(muPlusPt > %0.1f) && (muMinusPt > %0.1f) && (upsRapidity > %0.1f) && (upsRapidity < %0.1f)' % (opts.pt, opts.pt, opts.ymin, opts.ymax)

##cuts = '(muPlusPt > %0.1f) && (muMinusPt > %0.1f) && (abs(upsRapidity)<2.4)' \ 
simparamfile = opts.paramfile
## cuts = '(muPlusPt > 3.5) && (muMinusPt > 3.5) && (abs(upsRapidity)<2.4)'
## simparamfile = 'nom3.5SimFit.txt'

ws = RooWorkspace("ws","ws")

readData(ws, hidatafile, ppdatafile, cuts)

setBkgdPbPb = opts.bkgd_PbPb
setBkgdpp = opts.bkgd_pp
trkRotBkgd = opts.trkRot

buildPdf(ws, True, setBkgdPbPb, trkRotBkgd)  # pdf for PbPb
buildPdf(ws, False, setBkgdpp, False)   # pdf for pp
simPdf = buildSimPdf(ws, ws.cat('dataCat'))

mass = ws.var('invariantMass')
upsRap = ws.var('Rapidity')
##upsRap.setRange("RapidityBin",'%0.1f' % (opts.ymin),'%0.1f' % ( opts.ymax))
data = ws.data('data').reduce('(QQsign==QQsign::PlusMinus)&&(%s>%0.1f)&&(%s<%0.1f)' % (mass.GetName(), mmin, mass.GetName(), mmax))
##data = ws.data('data').reduce('(QQsign==QQsign::PlusMinus)')
data_ls = ws.data('data').reduce('(QQsign!=0)')
##data = ws.data('data').reduce('(QQsign==QQsign::PlusMinus)&&(%s>%0.1f)&&(%s<%0.1f)' % (upsRap, opts.ymin, upsRap, opts.ymax))
##data_ls = ws.data('data').reduce('(QQsign!=0)&&(%s>%0.1f)&&(%s<%0.1f)' % (upsRap, opts.ymin, upsRap, opts.ymax))

mass.setRange("fitRange",8.5,11.5)
mass.setRange(8.5,11.5)
##parsRap = pdf.getParameters(data)
pars = simPdf.getParameters(data)
#data_hi =  ws.data('data').reduce('(QQsign==QQsign::PlusMinus) && (dataCat == dataCat::hi)')
#data_hi.SetName('data_heavy')
#getattr(ws,'import')(data_hi)
ws.Print()
data.Print()

pars.readFromFile(simparamfile)
##parsRap.readFromFile

fr = simPdf.fitTo(data, RooFit.Extended(),
                  RooFit.Minos(False),
                  #RooFit.Range("fitRange"),
                  RooFit.Save(True))

pars.writeToFile('%s/lastSimFit.txt' % date)
#pars.writeToFile('%s/RaplastSimFit.txt' % date)
dataCat = ws.cat('dataCat')
catSet = RooArgSet(dataCat)

hican = TCanvas("hi", "hi")
#rapcan = TCanvas("hiRap","hiRap")

dataCat.setLabel('hi')

##dataCat.setLabel('hiRap')
mf_hi = mass.frame(7, 14, 70)
#mf_hi.addObject(fr.floatParsFinal())
#data_ls.plotOn(mf_hi,
#            RooFit.CutRange('fitRange'),
#            RooFit.Cut('dataCat==dataCat::hi'),
#            RooFit.MarkerColor(kRed),
#            RooFit.Name('data_hi_ls'))
data.plotOn(mf_hi,
            # RooFit.CutRange('fitRange'),
            RooFit.Cut('dataCat==dataCat::hi'),
            RooFit.Name('data_hi')
			)
simPdf.plotOn(mf_hi, RooFit.Slice(catSet),
			RooFit.Range('fitRange'),
				  RooFit.ProjWData(catSet,data),
				  RooFit.Name('fullFit')
				  )
simPdf.plotOn(mf_hi, RooFit.Components('bkg*'),
			RooFit.Range('fitRange'),
	         RooFit.Slice(catSet),
	         RooFit.ProjWData(catSet,data),
              #RooFit.LineColor(kRed+2),
			RooFit.LineStyle(kDashed))
simPdf.plotOn(mf_hi, RooFit.Components('bkgPoly*'),
			RooFit.Range('fitRange'),
	             RooFit.Slice(catSet),
	         RooFit.ProjWData(catSet,data),
	         RooFit.LineColor(kGreen+2),
         RooFit.LineStyle(kDashed))
simPdf.plotOn(mf_hi, RooFit.Components('bkgLikeSignPdf*'),
              RooFit.Range('fitRange'),
	             RooFit.Slice(catSet),
	         RooFit.ProjWData(catSet,data_ls),
	         RooFit.LineColor(kRed+2),
         RooFit.LineStyle(kDashed))
simPdf.paramOn(mf_hi,RooFit.Parameters(pars))
#
	##Pdf.plotOn(mf_hi, RooFit.Slice(catSet),
              # RooFit.Range('fitRange'),
	#         RooFit.ProjWData(catSet,data),
	#         RooFit.Name('fullFit')
	#         )
	#Pdf.plotOn(mf_hi, RooFit.Components('bkg*'),
              # RooFit.Range('fitRange'),
	#         RooFit.Slice(catSet),
	#         RooFit.ProjWData(catSet,data),
              #RooFit.LineColor(kRed+2),
#         RooFit.LineStyle(kDashed))
	#pdf.plotOn(mf_hi, RooFit.Components('bkgPoly*'),
              # RooFit.Range('fitRange'),
	#         RooFit.Slice(catSet),
	#         RooFit.ProjWData(catSet,data),
	#         RooFit.LineColor(kGreen+2),
#         RooFit.LineStyle(kDashed))
	#pdf.plotOn(mf_hi, RooFit.Components('bkgLikeSignPdf*'),
	#         # RooFit.Range('fitRange'),
	#         RooFit.Slice(catSet),
	#         RooFit.ProjWData(catSet,data),
	#         RooFit.LineColor(kRed+2),
#         RooFit.LineStyle(kDashed))

mf_hi.GetXaxis().SetTitle("m_{#mu^{+}#mu^{-}} (GeV/c^{2})");
mf_hi.GetXaxis().CenterTitle(True);
mf_hi.Draw()


##pyroot_logon.cmsPrelimHI(hican, 150)
##hican.Update()
##pyroot_logon.cmsPrelimHI(rapcan, 150)
##rapcan.Update()

ppcan = TCanvas("pp", "pp")
dataCat.setLabel('pp')
mf_pp = mass.frame(7, 14., 70)
data.plotOn(mf_pp,
            # RooFit.CutRange('fitRange'),
            RooFit.Cut('dataCat==dataCat::pp'),
            RooFit.Name('data_pp'))
simPdf.plotOn(mf_pp, RooFit.Slice(catSet),
              # RooFit.Range('fitRange'),
              RooFit.ProjWData(catSet,data),
              RooFit.Name('fullFit')
              )
simPdf.plotOn(mf_pp, RooFit.Components('bkg*'),
              # RooFit.Range('fitRange'),
              RooFit.Slice(catSet),
              RooFit.ProjWData(catSet,data),
              #RooFit.LineColor(kRed+2),
              RooFit.LineStyle(kDashed))
simPdf.plotOn(mf_pp, RooFit.Components('bkgPoly*'),
              # RooFit.Range('fitRange'),
              RooFit.Slice(catSet),
              RooFit.ProjWData(catSet,data),
              RooFit.LineColor(kGreen+2),
              RooFit.LineStyle(kDashed))
#simPdf.paramOn(mf_pp,RooFit.Parameters(pars))
mf_pp.GetXaxis().SetTitle("m_{#mu^{+}#mu^{-}} (GeV/c^{2})");
mf_pp.GetXaxis().CenterTitle(True);
mf_pp.Draw()
##pyroot_logon.cmsPrelimPP(ppcan, 230)
##ppcan.Update()


hican.Print('%s/hiFit%s_%s_%s_%s.png' % (date,opts.bkgd_PbPb,opts.bkgd_pp,opts.ymin,opts.ymax))
hican.Print('%s/hiFit%s_%s_%s_%s.pdf' % (date,opts.bkgd_PbPb,opts.bkgd_pp,opts.ymin,opts.ymax))
ppcan.Print('%s/ppFit%s_%s_%s_%s.png' % (date,opts.bkgd_PbPb,opts.bkgd_pp,opts.ymin,opts.ymax))
ppcan.Print('%s/ppFit%s_%s_%s_%s.pdf' % (date,opts.bkgd_PbPb,opts.bkgd_pp,opts.ymin,opts.ymax))

fr.Print('v')
#devError = computeRatioError(ws.var('f23_hi'), ws.var('f23_pp'),
#                             fr.correlation('f23_hi','f23_pp'))
#print
#print 'double ratios (hi/pp)'
#print '---------------------'
#print '(2S+3S)/1S : %0.3f +/- %0.3f' % (computeRatio(ws.var('f23_hi'),
#                                                     ws.var('f23_pp')),
#                                        devError)
#print '%s : %0.3f +/- %0.3f' % (ws.var('f2_pp').GetTitle(),
#                                computeRatio(ws.var('f2_hi'),
#                                             ws.var('f2_pp')),
#                                computeRatioError(ws.var('f2_hi'),
#                                                  ws.var('f2_pp'),
#                                                  fr.correlation('f2_hi','f2_pp')))
#print '---------------------'
#print

#deviation = 1.0 - computeRatio(ws.var('f23_hi'),ws.var('f23_pp'))
#print 'back of the envelope significance'
#print '---------------------------------'
#print '(2S+3S)/1S : %0.3f/%0.3f = %0.2f' % (deviation , devError,
#                                            deviation/devError)
#print '---------------------------------'
#print
#nsig1_hi = fr.floatParsFinal().find("nsig1_hi").getValV()
#nsig2_hi = fr.floatParsFinal().find("nsig2_hi").getValV()
#nsig3_hi = fr.floatParsFinal().find("nsig3_hi").getValV()
#nsig1_pp = fr.floatParsFinal().find("nsig1_pp").getValV()
#nbkg_hi = fr.floatParsFinal().find("nbkg_hi").getValV()
#nbkg_pp = fr.floatParsFinal().find("nbkg_pp").getValV()
#nsig1_hi_err = fr.floatParsFinal().find("nsig1_hi").getError()
#nsig2_hi_err = fr.floatParsFinal().find("nsig2_hi").getError()
#nsig3_hi_err = fr.floatParsFinal().find("nsig3_hi").getError()
#nsig1_pp_err = fr.floatParsFinal().find("nsig1_pp").getError()
#nbkg_hi_err = fr.floatParsFinal().find("nbkg_hi").getError()
#nbkg_pp_err = fr.floatParsFinal().find("nbkg_pp").getError()
chi2_hi = mf_hi.chiSquare('fullFit', 'data_hi', 0)*70
chi2_pp = mf_pp.chiSquare('fullFit', 'data_pp', 0)*70
chi2 = chi2_hi+chi2_pp
ndf = 70*2 - fr.floatParsFinal().getSize()
nll = fr.minNll();
#print '2S ratio = %0.3f/%0.3f = %0.3f' % (nsig2_hi, nsig1_hi, nsig2_hi/nsig1_hi)
#print '3S ratio = %0.3f/%0.3f = %0.3f' % (nsig3_hi, nsig1_hi, nsig3_hi/nsig1_hi)
print 'chi2/ndf = (%0.3f + %0.3f)/%i = %0.3f' % (chi2_hi, chi2_pp, ndf, chi2/ndf)
print 'chi2 prob = %0.4f' % (TMath.Prob(chi2, ndf))
print 'min nll = %0.8f' % (nll)


#output = open('%s/%s' % (date,opts.outputfile), "a")
#print >>output,'(ymin = %0.1f, ymax = %0.1f, bkgd_PbPb=%i, bkgd_pp=%i)' % (opts.ymin,opts.ymax, opts.bkgd_PbPb, opts.bkgd_pp)
#print >>output,'...'
#print >>output,'2S ratio = %0.3f/%0.3f = %0.3f' % (nsig2_hi, nsig1_hi, nsig2_hi/nsig1_hi)
#print >>output,'3S ratio = %0.3f/%0.3f = %0.3f' % (nsig3_hi, nsig1_hi, nsig3_hi/nsig1_hi)
#print >>output,'1S pp yield = %0.1f +/- %0.3f'% (nsig1_pp,nsig1_pp_err)
#print >>output,'PbPb bkg = %0.1f +/- %0.3f' % (nbkg_hi,nbkg_hi_err)
#print >>output,'pp bkg = %0.1f +/- %0.3f' % (nbkg_pp,nbkg_pp_err)
#print >>output,'chi2/ndf = (%0.3f + %0.3f)/%i = %0.3f' % (chi2_hi, chi2_pp, ndf, chi2/ndf)
#print >>output,'chi2 prob = %0.4f' % (TMath.Prob(chi2, ndf))
#print >>output,'min nll = %0.8f' % (nll)
#print >>output, "\n"

#fr.floatParsFinal()>>output
#output.close()

resultFile = TFile("lastSimFit3over2.root", "recreate")
fr.Write("nll")
ws.Write("ws")
resultFile.Close()




#frap = simPdf.fitTo(data_rap, RooFit.Extended(),
#		RooFit.Range("fitRange"),
#		RooFit.Minos(False),
#		RooFit.Save(True))
#		
#rapcan = TCanvas("hiRap","hiRap")
#dataCat.setLabel('hiRap')
#rf_hi = Rapidity.frame(ymin,ymax)

#data_rap.plotOn(rf_hi,
            # RooFit.CutRange('fitRange'),
#            RooFit.Cut('dataCat==dataCat::hiRap'),
#            RooFit.Name('data_rap'))

#
#rf_hi.GetXaxis().SetTitle("Rapidity");
#rf_hi.GetXaxis().CenterTitle(True);
#rf_hi.Draw()		

#pyroot_logon.cmsPrelimHI(rapcan, 150)
#rapcan.Update()

		
