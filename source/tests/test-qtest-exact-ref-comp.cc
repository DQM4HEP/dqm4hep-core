/// \file test-qtest-exact-ref-comp.cc
/*
 *
 * test-qtest-exact-ref-comp.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/PluginManager.h>

#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    exit(1);                                                                                                           \
  }
  
void fillExact(TH1 *histogram) {
  histogram->Fill(1);
  histogram->Fill(50);
  histogram->Fill(50);
  histogram->Fill(53);
  histogram->Fill(42);
  histogram->Fill(-2); // underflow
  histogram->Fill(-6); // underflow
  histogram->Fill(102);// overflow
  histogram->Fill(105);// overflow
  histogram->Fill(106);// overflow
}

void fillNotExact(TH1 *histogram) {
  histogram->Fill(2);
  histogram->Fill(40);
  histogram->Fill(50);
  histogram->Fill(23);
  histogram->Fill(22);
  histogram->Fill(-1); // underflow
  histogram->Fill(-3); // underflow
  histogram->Fill(106);// overflow
}

void fillExact(TGraph *graph) {
  graph->SetPoint(0, 12, 75);
  graph->SetPoint(1, 17, 45);
  graph->SetPoint(2, 25, 73);
  graph->SetPoint(3, 30, 12);
}

void fillNotExact(TGraph *graph) {
  graph->SetPoint(0, 112, 75);
  graph->SetPoint(1, 117, 45);
  graph->SetPoint(2, 125, 73);
  graph->SetPoint(3, 130, 78);
}

void fillDifferentNPoints(TGraph *graph) {
  graph->SetPoint(0, 12, 75);
  graph->SetPoint(1, 17, 45);
  graph->SetPoint(2, 25, 73);
  graph->SetPoint(3, 30, 12);
  graph->SetPoint(4, 112, 75);
  graph->SetPoint(5, 117, 45);
  graph->SetPoint(6, 125, 73);
  graph->SetPoint(7, 130, 78);
}

int main(int /*argc*/, char ** /*argv*/) {
  
  DQM4HEP_NO_EXCEPTION( Logger::createLogger("test-qtest-exact-ref-comp", {Logger::coloredConsole()}); );
  Logger::setMainLogger("test-qtest-exact-ref-comp");
  Logger::setLogLevel(spdlog::level::debug);

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  // create our test element - TH1
  MonitorElementPtr testElement;
  meMgr->bookHisto<TH1F>("/", "TestHisto", "A test histogram", testElement, 100, 0.f, 99.f);
  TH1F *histogram = testElement->objectTo<TH1F>();
  assert_test(nullptr != histogram);
  fillExact(histogram);

  // create different references
  PtrHandler<TObject> reference(new TH1F("", "The good reference", 100, 0.f, 99.f), true);
  fillExact((TH1F*)reference.ptr());
  PtrHandler<TObject> badReferenceBining(new TH1F("", "The bad reference - Wrong binning", 110, 0.f, 99.f), true);
  fillExact((TH1F*)badReferenceBining.ptr());
  PtrHandler<TObject> badReferenceContent(new TH1F("", "The bad reference - Wrong content", 100, 0.f, 99.f), true);
  fillNotExact((TH1F*)badReferenceContent.ptr());
  PtrHandler<TObject> badReferenceType(new TH1D("", "The bad reference - Wrong type", 100, 0.f, 99.f), true);
  fillExact((TH1F*)badReferenceType.ptr());

  // create the qtest XML config
  const std::string qtestType("ExactRefCompareTest");
  const std::string qtestName("UnitTestExactRefComp");
  TiXmlElement *qtestElement = new TiXmlElement("qtest");
  std::shared_ptr<TiXmlElement> sharedQTest(qtestElement);
  qtestElement->SetAttribute("name", qtestName);
  qtestElement->SetAttribute("type", qtestType);

  // configure the qtest
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(qtestElement));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement->path(), testElement->name(), qtestName));
  
  // run it !
  QReportStorage storage; QReport report; json jsonReport;
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID); // no reference assigned yet
  
  storage.clear();
  testElement->setReferenceObject(reference);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS); // same histograms
  
  storage.clear();
  testElement->setReferenceObject(badReferenceBining);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID); // reference has a different binning
  
  storage.clear();
  testElement->setReferenceObject(badReferenceType);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID); // wrong reference type
  
  storage.clear();
  testElement->setReferenceObject(badReferenceContent);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == ERROR); // valid test but reference is different
  
  assert_test(STATUS_CODE_SUCCESS == meMgr->removeMonitorElement(testElement->path(), testElement->name()));
  
  // create our test element - TScalar
  meMgr->bookScalar<int>("/", "TestScalar", "A test int", testElement, 42);
  TScalarInt *integer = testElement->objectTo<TScalarInt>();
  assert_test(nullptr != integer);
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement->path(), testElement->name(), qtestName));
  
  reference.set(new TScalarInt(42), true);
  badReferenceContent.set(new TScalarInt(45), true);
  badReferenceType.set(new TScalarFloat(4.f), true);
  
  storage.clear();
  testElement->setReferenceObject(reference);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS); // same scalar values
  
  storage.clear();
  testElement->setReferenceObject(badReferenceContent);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == ERROR); // different scalar values
  
  storage.clear();
  testElement->setReferenceObject(badReferenceType);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID); // wrong reference type

  assert_test(STATUS_CODE_SUCCESS == meMgr->removeMonitorElement(testElement->path(), testElement->name()));
  
  // create our test element - TGraph (1D)
  meMgr->bookObject<TGraph>("/", "TestGraph", "A test graph", testElement);
  TGraph *graph = testElement->objectTo<TGraph>();
  assert_test(nullptr != graph);
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement->path(), testElement->name(), qtestName));
  fillExact(graph);
  
  reference.set(new TGraph(), true);
  fillExact((TGraph*)reference.ptr());
  badReferenceBining.set(new TGraph(), true);
  fillDifferentNPoints((TGraph*)badReferenceBining.ptr());
  badReferenceContent.set(new TGraph(), true);
  fillNotExact((TGraph*)badReferenceContent.ptr());
  badReferenceType.set(new TGraphErrors(), true);
  fillExact((TGraph*)badReferenceType.ptr());
  
  storage.clear();
  testElement->setReferenceObject(reference);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS); // same graphs

  storage.clear();
  testElement->setReferenceObject(badReferenceBining);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID); // reference has a different n points
  
  storage.clear();
  testElement->setReferenceObject(badReferenceType);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID); // wrong reference type
  
  storage.clear();
  testElement->setReferenceObject(badReferenceContent);
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == ERROR); // valid test but reference is different
  
  return 0;
}
