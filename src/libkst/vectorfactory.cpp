/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "vectorfactory.h"

#include "debug.h"
#include "vector.h"
#include "generatedvector.h"
#include "editablevector.h"
#include "datavector.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

VectorFactory::VectorFactory()
: PrimitiveFactory() {
  registerFactory(Vector::staticTypeTag, this);
}


VectorFactory::~VectorFactory() {
}


PrimitivePtr VectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;
  QString descriptiveName;
  Q_ASSERT(store);

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == Vector::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == Vector::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  VectorPtr vector = store->createObject<Vector>();
  vector->change(data);
  vector->setDescriptiveName(descriptiveName);

  vector->writeLock();
  vector->update();
  vector->unlock();

  return vector;
}


GeneratedVectorFactory::GeneratedVectorFactory()
: PrimitiveFactory() {
  registerFactory(GeneratedVector::staticTypeTag, this);
}


GeneratedVectorFactory::~GeneratedVectorFactory() {
}


PrimitivePtr GeneratedVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  double min=-1.0, max=1.0;
  int count=0;
  QString descriptiveName;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == GeneratedVector::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        min = attrs.value("min").toString().toDouble();
        max = attrs.value("max").toString().toDouble();
        count = attrs.value("count").toString().toInt();
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == GeneratedVector::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating generated vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  GeneratedVectorPtr vector = store->createObject<GeneratedVector>();
  vector->changeRange(min, max, count);
  vector->setDescriptiveName(descriptiveName);

  vector->writeLock();
  vector->update();
  vector->unlock();

  return vector;
}

EditableVectorFactory::EditableVectorFactory()
: PrimitiveFactory() {
  registerFactory(EditableVector::staticTypeTag, this);
}


EditableVectorFactory::~EditableVectorFactory() {
}


PrimitivePtr EditableVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;
  QString descriptiveName;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == EditableVector::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else if (n == "data") {
        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == EditableVector::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  EditableVectorPtr vector = store->createObject<EditableVector>();
  vector->change(data);
  vector->setDescriptiveName(descriptiveName);

  vector->writeLock();
  vector->update();
  vector->unlock();

  return vector;
}


DataVectorFactory::DataVectorFactory()
: PrimitiveFactory() {
  registerFactory(DataVector::staticTypeTag, this);
}


DataVectorFactory::~DataVectorFactory() {
}


PrimitivePtr DataVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;
  QString provider, file, field;
  QString descriptiveName;
  int start=0, count=0, skip = -1;
  bool doAve=false;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == DataVector::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();

        provider = attrs.value("provider").toString();
        file = attrs.value("file").toString();
        field = attrs.value("field").toString();
        start = attrs.value("start").toString().toInt();
        count = attrs.value("count").toString().toInt();
        skip = attrs.value("skip").toString().toInt();
        doAve = attrs.value("doAve").toString() == "true" ? true : false;
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);

      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == DataVector::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  Q_ASSERT(store);
  DataSourcePtr dataSource = DataSource::findOrLoadSource(store, file);

  if (!dataSource) {
    return 0; //Couldn't find a suitable datasource
  }

  DataVectorPtr vector = store->createObject<DataVector>();

  vector->writeLock();
  vector->change(dataSource, field,
      start,
      count,
      skip,
      (skip != -1),
      doAve);

  vector->setDescriptiveName(descriptiveName);
  vector->update();
  vector->unlock();

  return vector;
}

}

// vim: ts=2 sw=2 et
